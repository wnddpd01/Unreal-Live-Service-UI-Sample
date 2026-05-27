#include "K2Node_SendViewEvent.h"

#include "DefaultView.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "Engine/Blueprint.h"
#include "EdGraph/EdGraph.h"
#include "EdGraphSchema_K2.h"
#include "Kismet2/CompilerResultsLog.h"

namespace
{
    bool IsDefaultViewBlueprintGraph(const UEdGraph* Graph)
    {
        const UBlueprint* Blueprint = Graph ? Graph->GetTypedOuter<UBlueprint>() : nullptr;
        if (!Blueprint)
        {
            return false;
        }

        const UClass* BlueprintClass =
            Blueprint->GeneratedClass ? Blueprint->GeneratedClass.Get() :
            Blueprint->SkeletonGeneratedClass.Get();
        return BlueprintClass &&
            BlueprintClass->IsChildOf(UDefaultView::StaticClass());
    }
}

const FName UK2Node_SendViewEvent::EventIdPinName(TEXT("EventId"));

void UK2Node_SendViewEvent::AllocateDefaultPins()
{
    if (FunctionReference.GetMemberName().IsNone())
    {
        SetFromFunction(UDefaultView::StaticClass()->FindFunctionByName(
            GET_FUNCTION_NAME_CHECKED(UDefaultView, SendViewEvent)
        ));
    }

    Super::AllocateDefaultPins();

    if (UEdGraphPin* EventIdPin = FindPin(EventIdPinName, EGPD_Input))
    {
        EventIdPin->bNotConnectable = true;
    }
}

FText UK2Node_SendViewEvent::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    return NSLOCTEXT("UISampleEditor", "SendViewEventNodeTitle", "Send View Event");
}

FText UK2Node_SendViewEvent::GetTooltipText() const
{
    return NSLOCTEXT(
        "UISampleEditor",
        "SendViewEventNodeTooltip",
        "Sends a view event declared on this DefaultView Blueprint."
    );
}

FText UK2Node_SendViewEvent::GetMenuCategory() const
{
    return NSLOCTEXT("UISampleEditor", "UIViewMenuCategory", "UI|View");
}

bool UK2Node_SendViewEvent::CanPasteHere(const UEdGraph* TargetGraph) const
{
    return IsDefaultViewBlueprintGraph(TargetGraph);
}

void UK2Node_SendViewEvent::GetMenuActions(
    FBlueprintActionDatabaseRegistrar& ActionRegistrar
) const
{
    UClass* ActionKey = GetClass();
    if (!ActionRegistrar.IsOpenForRegistration(ActionKey))
    {
        return;
    }

    UBlueprintNodeSpawner* NodeSpawner =
        UBlueprintNodeSpawner::Create(GetClass());
    ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
}

bool UK2Node_SendViewEvent::IsActionFilteredOut(
    const FBlueprintActionFilter& Filter
)
{
    for (const UBlueprint* Blueprint : Filter.Context.Blueprints)
    {
        if (!Blueprint || !Blueprint->GeneratedClass ||
            !Blueprint->GeneratedClass->IsChildOf(UDefaultView::StaticClass()))
        {
            return true;
        }
    }

    for (const UEdGraph* Graph : Filter.Context.Graphs)
    {
        if (!IsDefaultViewBlueprintGraph(Graph))
        {
            return true;
        }
    }

    return false;
}

void UK2Node_SendViewEvent::ValidateNodeDuringCompilation(
    FCompilerResultsLog& MessageLog
) const
{
    Super::ValidateNodeDuringCompilation(MessageLog);

    const UEdGraphPin* EventIdPin = FindPin(EventIdPinName, EGPD_Input);
    if (!EventIdPin)
    {
        return;
    }

    const FName EventId(*EventIdPin->DefaultValue);
    if (!IsDeclaredEventId(EventId))
    {
        MessageLog.Error(
            *FString::Printf(
                TEXT("@@ sends undeclared view event '%s'. Add it to DeclaredViewEvents first."),
                *EventIdPin->DefaultValue
            ),
            this
        );
    }
}

TArray<FName> UK2Node_SendViewEvent::GetDeclaredViewEventNames() const
{
    TArray<FName> EventNames;

    const UBlueprint* Blueprint = GetTypedOuter<UBlueprint>();
    const UClass* ViewClass = Blueprint ? Blueprint->GeneratedClass.Get() : nullptr;
    const UDefaultView* DefaultView =
        ViewClass ? Cast<UDefaultView>(ViewClass->GetDefaultObject(false)) : nullptr;
    if (!DefaultView)
    {
        return EventNames;
    }

    const FName ViewId = DefaultView->GetViewId();
    for (const FName& EventName : DefaultView->GetDeclaredViewEvents())
    {
        const FName NormalizedEventName =
            NormalizeViewEventName(ViewId, EventName);
        if (!NormalizedEventName.IsNone())
        {
            EventNames.AddUnique(NormalizedEventName);
        }
    }

    EventNames.Sort([](const FName& Left, const FName& Right)
        {
            return Left.LexicalLess(Right);
        });
    return EventNames;
}

bool UK2Node_SendViewEvent::IsDeclaredEventId(FName EventId) const
{
    if (EventId.IsNone())
    {
        return false;
    }

    const UBlueprint* Blueprint = GetTypedOuter<UBlueprint>();
    const UClass* ViewClass = Blueprint ? Blueprint->GeneratedClass.Get() : nullptr;
    const UDefaultView* DefaultView =
        ViewClass ? Cast<UDefaultView>(ViewClass->GetDefaultObject(false)) : nullptr;
    if (!DefaultView)
    {
        return true;
    }

    const FName ViewId = DefaultView->GetViewId();
    const FName EventName = NormalizeViewEventName(ViewId, EventId);
    for (const FName& DeclaredEventName : DefaultView->GetDeclaredViewEvents())
    {
        if (NormalizeViewEventName(ViewId, DeclaredEventName) == EventName)
        {
            return true;
        }
    }

    return false;
}

FName UK2Node_SendViewEvent::MakeViewEventId(FName ViewId, FName EventName)
{
    if (ViewId.IsNone() || EventName.IsNone())
    {
        return NAME_None;
    }

    return FName(*FString::Printf(
        TEXT("View.%s.%s"),
        *ViewId.ToString(),
        *EventName.ToString()
    ));
}

FName UK2Node_SendViewEvent::NormalizeViewEventName(
    FName ViewId,
    FName EventId
)
{
    if (ViewId.IsNone() || EventId.IsNone())
    {
        return NAME_None;
    }

    const FString EventIdString = EventId.ToString();
    const FString ViewPrefix = FString::Printf(TEXT("View.%s."), *ViewId.ToString());
    if (EventIdString.StartsWith(ViewPrefix))
    {
        return FName(*EventIdString.RightChop(ViewPrefix.Len()));
    }

    const FString LegacyPrefix = FString::Printf(TEXT("%s."), *ViewId.ToString());
    if (EventIdString.StartsWith(LegacyPrefix))
    {
        return FName(*EventIdString.RightChop(LegacyPrefix.Len()));
    }

    if (!EventIdString.Contains(TEXT(".")))
    {
        return EventId;
    }

    return NAME_None;
}
