#include "K2Node_SendViewEvent.h"
#include "DefaultView.h"
#include "EdGraphUtilities.h"
#include "Engine/Blueprint.h"
#include "K2Node_CallFunction.h"
#include "Modules/ModuleManager.h"
#include "SGraphPinNameList.h"

namespace
{
    FName NormalizeViewEventName(FName ViewId, FName EventId)
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

    bool IsSendViewEventPin(const UEdGraphPin* Pin)
    {
        if (!Pin || Pin->PinName != UK2Node_SendViewEvent::EventIdPinName)
        {
            return false;
        }

        const UK2Node_CallFunction* CallFunctionNode =
            Cast<UK2Node_CallFunction>(Pin->GetOwningNode());
        return CallFunctionNode &&
            CallFunctionNode->GetFunctionName() ==
            GET_FUNCTION_NAME_CHECKED(UDefaultView, SendViewEvent);
    }

    TArray<FName> GetDeclaredViewEventNames(const UEdGraphPin* Pin)
    {
        TArray<FName> EventNames;

        const UEdGraphNode* OwningNode = Pin ? Pin->GetOwningNode() : nullptr;
        const UBlueprint* Blueprint =
            OwningNode ? OwningNode->GetTypedOuter<UBlueprint>() : nullptr;
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
}

class FUIViewEventPinFactory : public FGraphPanelPinFactory
{
public:
    virtual TSharedPtr<SGraphPin> CreatePin(UEdGraphPin* Pin) const override
    {
        if (!IsSendViewEventPin(Pin))
        {
            return nullptr;
        }

        TArray<TSharedPtr<FName>> NameList;
        for (const FName& EventName : GetDeclaredViewEventNames(Pin))
        {
            NameList.Add(MakeShared<FName>(EventName));
        }

        return SNew(SGraphPinNameList, Pin, NameList);
    }
};

class FUISampleEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override
    {
        ViewEventPinFactory = MakeShared<FUIViewEventPinFactory>();
        FEdGraphUtilities::RegisterVisualPinFactory(ViewEventPinFactory);
    }

    virtual void ShutdownModule() override
    {
        if (ViewEventPinFactory.IsValid())
        {
            FEdGraphUtilities::UnregisterVisualPinFactory(ViewEventPinFactory);
            ViewEventPinFactory.Reset();
        }
    }

private:
    TSharedPtr<FUIViewEventPinFactory> ViewEventPinFactory;
};

IMPLEMENT_MODULE(FUISampleEditorModule, UISampleEditor)
