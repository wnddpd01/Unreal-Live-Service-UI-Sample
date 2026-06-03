#include "K2Node_SendViewEvent.h"
#include "DefaultView.h"
#include "EdGraphUtilities.h"
#include "Engine/Blueprint.h"
#include "K2Node_CallFunction.h"
#include "Modules/ModuleManager.h"
#include "SGraphPinNameList.h"

namespace
{
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

    TArray<FName> GetDeclaredViewEventIds(const UEdGraphPin* Pin)
    {
        TArray<FName> EventIds;

        const UEdGraphNode* OwningNode = Pin ? Pin->GetOwningNode() : nullptr;
        const UBlueprint* Blueprint =
            OwningNode ? OwningNode->GetTypedOuter<UBlueprint>() : nullptr;
        const UClass* ViewClass = Blueprint ? Blueprint->GeneratedClass.Get() : nullptr;
        const UDefaultView* DefaultView =
            ViewClass ? Cast<UDefaultView>(ViewClass->GetDefaultObject(false)) : nullptr;
        if (!DefaultView)
        {
            return EventIds;
        }

        for (const FName& EventId : DefaultView->GetDeclaredViewEvents())
        {
            if (!EventId.IsNone())
            {
                EventIds.AddUnique(EventId);
            }
        }

        EventIds.Sort([](const FName& Left, const FName& Right)
            {
                return Left.LexicalLess(Right);
            });
        return EventIds;
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
        for (const FName& EventId : GetDeclaredViewEventIds(Pin))
        {
            NameList.Add(MakeShared<FName>(EventId));
        }

        return SNew(SGraphPinNameList, Pin, NameList);
    }
};

class FPrismUIEditorModule : public IModuleInterface
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

IMPLEMENT_MODULE(FPrismUIEditorModule, PrismUIEditor)
