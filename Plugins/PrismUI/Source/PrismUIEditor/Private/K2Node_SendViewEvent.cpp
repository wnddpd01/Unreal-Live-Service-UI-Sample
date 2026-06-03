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
    return NSLOCTEXT("PrismUIEditor", "SendViewEventNodeTitle", "Send View Event");
}

FText UK2Node_SendViewEvent::GetTooltipText() const
{
    return NSLOCTEXT(
        "PrismUIEditor",
        "SendViewEventNodeTooltip",
        "Sends a view event declared on this DefaultView Blueprint."
    );
}

FText UK2Node_SendViewEvent::GetMenuCategory() const
{
    return NSLOCTEXT("PrismUIEditor", "UIViewMenuCategory", "UI|View");
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

TArray<FName> UK2Node_SendViewEvent::GetDeclaredViewEventIds() const
{
    TArray<FName> EventIds;

    const UBlueprint* Blueprint = GetTypedOuter<UBlueprint>();
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

    for (const FName& DeclaredEventId : DefaultView->GetDeclaredViewEvents())
    {
        if (DeclaredEventId == EventId)
        {
            return true;
        }
    }

    return false;
}
