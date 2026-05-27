#include "CollectUIEventsCommandlet.h"

#include "DefaultView.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Dom/JsonObject.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraph/EdGraphPin.h"
#include "Engine/Blueprint.h"
#include "HAL/FileManager.h"
#include "K2Node_CallFunction.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

namespace
{
    FString ResolveKind(const FString& EventId)
    {
        FString Kind;
        FString Remainder;
        if (EventId.Split(TEXT("."), &Kind, &Remainder))
        {
            return Kind;
        }

        return TEXT("Unknown");
    }

    TSharedRef<FJsonObject> MakeEventObject(
        const FString& EventId,
        const FString& Name,
        const FString& Owner,
        const FString& Source,
        const FString& ViewId = FString()
    )
    {
        TSharedRef<FJsonObject> EventObject = MakeShared<FJsonObject>();
        EventObject->SetStringField(TEXT("id"), EventId);
        EventObject->SetStringField(TEXT("kind"), ResolveKind(EventId));
        EventObject->SetStringField(TEXT("name"), Name);
        EventObject->SetStringField(TEXT("owner"), Owner);
        EventObject->SetStringField(TEXT("source"), Source);

        if (!ViewId.IsEmpty())
        {
            EventObject->SetStringField(TEXT("viewId"), ViewId);
        }

        return EventObject;
    }

    bool AddEvent(
        TMap<FString, TSharedPtr<FJsonObject>>& EventsById,
        const TSharedRef<FJsonObject>& EventObject,
        FString& OutError
    )
    {
        const FString EventId = EventObject->GetStringField(TEXT("id"));
        if (EventId.IsEmpty())
        {
            OutError = TEXT("UIEvent entry has an empty id.");
            return false;
        }

        if (EventsById.Contains(EventId))
        {
            OutError = FString::Printf(TEXT("Duplicate UIEvent id '%s'."), *EventId);
            return false;
        }

        EventsById.Add(EventId, EventObject);
        return true;
    }

    FString MakeViewEventId(FName ViewId, FName EventName)
    {
        return FString::Printf(
            TEXT("View.%s.%s"),
            *ViewId.ToString(),
            *EventName.ToString()
        );
    }

    FString MakeEventName(const FString& EventId)
    {
        FString Left;
        FString Right;
        if (EventId.Split(TEXT("."), &Left, &Right, ESearchCase::CaseSensitive, ESearchDir::FromEnd))
        {
            return Right;
        }

        return EventId;
    }

    FString MakeOwnerPath(const FString& FilePath)
    {
        FString Owner = FPaths::ConvertRelativePathToFull(FilePath);
        FString ProjectDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
        FPaths::MakePathRelativeTo(Owner, *ProjectDir);
        return Owner;
    }

    FString RemoveCppComments(const FString& SourceText)
    {
        FString Result;
        Result.Reserve(SourceText.Len());

        bool bInLineComment = false;
        bool bInBlockComment = false;
        bool bInString = false;
        bool bInChar = false;
        bool bEscaped = false;

        for (int32 Index = 0; Index < SourceText.Len(); ++Index)
        {
            const TCHAR Current = SourceText[Index];
            const TCHAR Next = Index + 1 < SourceText.Len() ? SourceText[Index + 1] : TEXT('\0');

            if (bInLineComment)
            {
                if (Current == TEXT('\n'))
                {
                    bInLineComment = false;
                    Result.AppendChar(Current);
                }
                continue;
            }

            if (bInBlockComment)
            {
                if (Current == TEXT('*') && Next == TEXT('/'))
                {
                    bInBlockComment = false;
                    ++Index;
                }
                else if (Current == TEXT('\n'))
                {
                    Result.AppendChar(Current);
                }
                continue;
            }

            if (!bInString && !bInChar && Current == TEXT('/') && Next == TEXT('/'))
            {
                bInLineComment = true;
                ++Index;
                continue;
            }

            if (!bInString && !bInChar && Current == TEXT('/') && Next == TEXT('*'))
            {
                bInBlockComment = true;
                ++Index;
                continue;
            }

            Result.AppendChar(Current);

            if (bEscaped)
            {
                bEscaped = false;
                continue;
            }

            if ((bInString || bInChar) && Current == TEXT('\\'))
            {
                bEscaped = true;
                continue;
            }

            if (!bInChar && Current == TEXT('"'))
            {
                bInString = !bInString;
            }
            else if (!bInString && Current == TEXT('\''))
            {
                bInChar = !bInChar;
            }
        }

        return Result;
    }

    bool IsEventIdCharacter(TCHAR Character)
    {
        return FChar::IsAlnum(Character) ||
            Character == TEXT('_') ||
            Character == TEXT('.');
    }

    bool IsValidEventId(const FString& EventId)
    {
        if (EventId.IsEmpty() || !EventId.Contains(TEXT(".")))
        {
            return false;
        }

        for (const TCHAR Character : EventId)
        {
            if (!IsEventIdCharacter(Character))
            {
                return false;
            }
        }

        return true;
    }

    void FindMacroEventIds(
        const FString& SourceText,
        const FString& MacroName,
        TArray<FString>& OutEventIds
    )
    {
        int32 SearchStart = 0;
        while (SearchStart < SourceText.Len())
        {
            const int32 MacroIndex = SourceText.Find(
                MacroName,
                ESearchCase::CaseSensitive,
                ESearchDir::FromStart,
                SearchStart
            );
            if (MacroIndex == INDEX_NONE)
            {
                break;
            }

            const int32 AfterMacroIndex = MacroIndex + MacroName.Len();
            SearchStart = AfterMacroIndex;

            const TCHAR Previous =
                MacroIndex > 0 ? SourceText[MacroIndex - 1] : TEXT('\0');
            const TCHAR Next =
                AfterMacroIndex < SourceText.Len() ? SourceText[AfterMacroIndex] : TEXT('\0');
            if ((Previous != TEXT('\0') && (FChar::IsAlnum(Previous) || Previous == TEXT('_'))) ||
                (Next != TEXT('\0') && (FChar::IsAlnum(Next) || Next == TEXT('_'))))
            {
                continue;
            }

            const int32 OpenParenIndex = SourceText.Find(
                TEXT("("),
                ESearchCase::CaseSensitive,
                ESearchDir::FromStart,
                AfterMacroIndex
            );
            if (OpenParenIndex == INDEX_NONE)
            {
                continue;
            }

            FString BetweenMacroAndParen =
                SourceText.Mid(AfterMacroIndex, OpenParenIndex - AfterMacroIndex);
            BetweenMacroAndParen.TrimStartAndEndInline();
            if (!BetweenMacroAndParen.IsEmpty())
            {
                continue;
            }

            const int32 CloseParenIndex = SourceText.Find(
                TEXT(")"),
                ESearchCase::CaseSensitive,
                ESearchDir::FromStart,
                OpenParenIndex + 1
            );
            if (CloseParenIndex == INDEX_NONE)
            {
                continue;
            }

            FString EventId =
                SourceText.Mid(OpenParenIndex + 1, CloseParenIndex - OpenParenIndex - 1);
            EventId.TrimStartAndEndInline();
            if (IsValidEventId(EventId))
            {
                OutEventIds.Add(EventId);
            }

            SearchStart = CloseParenIndex + 1;
        }
    }

    bool CollectCppEventDeclarations(
        TMap<FString, TSharedPtr<FJsonObject>>& EventsById,
        FString& OutError
    )
    {
        TArray<FString> HeaderFiles;
        TArray<FString> CppFiles;
        TArray<FString> SourceFiles;
        const FString SourceDir = FPaths::Combine(FPaths::ProjectDir(), TEXT("Source"));
        IFileManager::Get().FindFilesRecursive(HeaderFiles, *SourceDir, TEXT("*.h"), true, false);
        IFileManager::Get().FindFilesRecursive(CppFiles, *SourceDir, TEXT("*.cpp"), true, false);
        SourceFiles.Append(HeaderFiles);
        SourceFiles.Append(CppFiles);
        UE_LOG(LogTemp, Display, TEXT("Scanning %d C++ source files for UI event declarations."), SourceFiles.Num());

        const FString MacroNames[] =
        {
            TEXT("UI_EVENT"),
            TEXT("UI_MODEL_EVENT"),
            TEXT("UI_VIEW_EVENT")
        };

        for (const FString& SourceFile : SourceFiles)
        {
            FString SourceText;
            if (!FFileHelper::LoadFileToString(SourceText, *SourceFile))
            {
                OutError = FString::Printf(TEXT("Failed to read C++ source file '%s'."), *SourceFile);
                return false;
            }

            const FString ScannedText = RemoveCppComments(SourceText);
            TArray<FString> EventIds;
            for (const FString& MacroName : MacroNames)
            {
                FindMacroEventIds(ScannedText, MacroName, EventIds);
            }

            for (const FString& EventId : EventIds)
            {
                if (!AddEvent(
                    EventsById,
                    MakeEventObject(
                        EventId,
                        MakeEventName(EventId),
                        MakeOwnerPath(SourceFile),
                        TEXT("C++")
                    ),
                    OutError
                ))
                {
                    return false;
                }
            }
        }

        return true;
    }

    FName NormalizeViewEventName(FName ViewId, const FString& RawEventId)
    {
        FString EventId = RawEventId;
        EventId.TrimStartAndEndInline();

        if (EventId.IsEmpty() || EventId == TEXT("None"))
        {
            return NAME_None;
        }

        const FString ViewPrefix = FString::Printf(TEXT("View.%s."), *ViewId.ToString());
        if (EventId.StartsWith(ViewPrefix))
        {
            return FName(*EventId.RightChop(ViewPrefix.Len()));
        }

        const FString LegacyPrefix = FString::Printf(TEXT("%s."), *ViewId.ToString());
        if (EventId.StartsWith(LegacyPrefix))
        {
            return FName(*EventId.RightChop(LegacyPrefix.Len()));
        }

        if (!EventId.Contains(TEXT(".")))
        {
            return FName(*EventId);
        }

        return NAME_None;
    }

    bool IsDeclaredViewEvent(
        FName ViewId,
        FName EventName,
        const TSet<FName>& DeclaredEventNames
    )
    {
        if (EventName.IsNone())
        {
            return false;
        }

        if (MakeViewEventId(ViewId, EventName) ==
            MakeViewEventId(ViewId, FName(TEXT("Constructed"))))
        {
            return true;
        }

        return DeclaredEventNames.Contains(EventName);
    }

    bool ValidateGraphViewEvents(
        const UBlueprint& Blueprint,
        FName ViewId,
        const TSet<FName>& DeclaredEventNames,
        FString& OutError
    )
    {
        TArray<UEdGraph*> Graphs;
        Graphs.Append(Blueprint.UbergraphPages);
        Graphs.Append(Blueprint.FunctionGraphs);
        Graphs.Append(Blueprint.MacroGraphs);

        for (const UEdGraph* Graph : Graphs)
        {
            if (!Graph)
            {
                continue;
            }

            for (const UEdGraphNode* Node : Graph->Nodes)
            {
                const UK2Node_CallFunction* CallFunctionNode =
                    Cast<UK2Node_CallFunction>(Node);
                if (!CallFunctionNode)
                {
                    continue;
                }

                const FName FunctionName = CallFunctionNode->GetFunctionName();
                if (FunctionName == FName(TEXT("EmitViewEvent")))
                {
                    OutError = FString::Printf(
                        TEXT("%s still calls removed EmitViewEvent. Replace it with SendViewEvent."),
                        *Blueprint.GetPathName()
                    );
                    return false;
                }

                if (FunctionName != GET_FUNCTION_NAME_CHECKED(UDefaultView, SendViewEvent))
                {
                    continue;
                }

                const UEdGraphPin* EventIdPin = CallFunctionNode->FindPin(TEXT("EventId"));
                if (!EventIdPin)
                {
                    continue;
                }

                const FName EventName =
                    NormalizeViewEventName(ViewId, EventIdPin->DefaultValue);
                if (!IsDeclaredViewEvent(ViewId, EventName, DeclaredEventNames))
                {
                    OutError = FString::Printf(
                        TEXT("%s sends undeclared view event '%s'. Add '%s' to DeclaredViewEvents first."),
                        *Blueprint.GetPathName(),
                        *EventIdPin->DefaultValue,
                        *EventName.ToString()
                    );
                    return false;
                }
            }
        }

        return true;
    }
}

UCollectUIEventsCommandlet::UCollectUIEventsCommandlet()
{
    IsClient = false;
    IsEditor = true;
    IsServer = false;
    LogToConsole = true;
}

int32 UCollectUIEventsCommandlet::Main(const FString& Params)
{
    TMap<FString, TSharedPtr<FJsonObject>> EventsById;

    FString CppScanError;
    if (!CollectCppEventDeclarations(EventsById, CppScanError))
    {
        UE_LOG(LogTemp, Error, TEXT("%s"), *CppScanError);
        return 1;
    }

    FAssetRegistryModule& AssetRegistryModule =
        FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
    IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
    AssetRegistry.SearchAllAssets(true);

    TArray<FAssetData> Assets;
    AssetRegistry.GetAssetsByPath(FName(TEXT("/Game")), Assets, true);

    for (const FAssetData& AssetData : Assets)
    {
        UObject* Asset = AssetData.GetAsset();
        UBlueprint* Blueprint = Cast<UBlueprint>(Asset);
        if (!Blueprint || !Blueprint->GeneratedClass ||
            !Blueprint->GeneratedClass->IsChildOf(UDefaultView::StaticClass()))
        {
            continue;
        }

        const UDefaultView* DefaultView =
            Cast<UDefaultView>(Blueprint->GeneratedClass->GetDefaultObject());
        if (!DefaultView || DefaultView->GetViewId().IsNone())
        {
            continue;
        }

        const FName ViewIdName = DefaultView->GetViewId();
        const FString ViewId = ViewIdName.ToString();
        const FString Owner = AssetData.PackageName.ToString();
        TSet<FName> ViewEventNames;

        for (const FName& EventName : DefaultView->GetDeclaredViewEvents())
        {
            const FName NormalizedEventName =
                NormalizeViewEventName(ViewIdName, EventName.ToString());
            if (!NormalizedEventName.IsNone())
            {
                ViewEventNames.Add(NormalizedEventName);
            }
        }

        FString ValidationError;
        if (!ValidateGraphViewEvents(
            *Blueprint,
            ViewIdName,
            ViewEventNames,
            ValidationError
        ))
        {
            UE_LOG(LogTemp, Error, TEXT("%s"), *ValidationError);
            return 1;
        }

        FString Error;
        if (!AddEvent(
            EventsById,
            MakeEventObject(
                MakeViewEventId(ViewIdName, FName(TEXT("Constructed"))),
                TEXT("Constructed"),
                Owner,
                TEXT("WBP"),
                ViewId
            ),
            Error
        ))
        {
            UE_LOG(LogTemp, Error, TEXT("%s"), *Error);
            return 1;
        }

        for (const FName& EventName : ViewEventNames)
        {
            if (EventName.IsNone())
            {
                continue;
            }

            if (!AddEvent(
                EventsById,
                MakeEventObject(
                    MakeViewEventId(ViewIdName, EventName),
                    EventName.ToString(),
                    Owner,
                    TEXT("WBP"),
                    ViewId
                ),
                Error
            ))
            {
                UE_LOG(LogTemp, Error, TEXT("%s"), *Error);
                return 1;
            }
        }
    }

    EventsById.KeySort([](const FString& Left, const FString& Right)
        {
            return Left < Right;
        });

    TArray<TSharedPtr<FJsonValue>> EventValues;
    for (const TPair<FString, TSharedPtr<FJsonObject>>& Pair : EventsById)
    {
        EventValues.Add(MakeShared<FJsonValueObject>(Pair.Value.ToSharedRef()));
    }

    TSharedRef<FJsonObject> RootObject = MakeShared<FJsonObject>();
    RootObject->SetNumberField(TEXT("version"), 1);
    RootObject->SetArrayField(TEXT("events"), EventValues);

    FString OutputJson;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputJson);
    if (!FJsonSerializer::Serialize(RootObject, Writer))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to serialize UIEvents.json."));
        return 1;
    }

    const FString OutputPath = FPaths::Combine(
        FPaths::ProjectDir(),
        TEXT("Source/UISample/UIEvents/UIEvents.json")
    );

    if (!FFileHelper::SaveStringToFile(OutputJson, *OutputPath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to write %s."), *OutputPath);
        return 1;
    }

    UE_LOG(LogTemp, Display, TEXT("Collected %d UI events into %s."), EventsById.Num(), *OutputPath);
    return 0;
}
