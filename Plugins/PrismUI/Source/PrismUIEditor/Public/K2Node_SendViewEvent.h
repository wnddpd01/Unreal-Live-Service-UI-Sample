#pragma once

#include "CoreMinimal.h"
#include "K2Node_CallFunction.h"
#include "K2Node_SendViewEvent.generated.h"

UCLASS()
class PRISMUIEDITOR_API UK2Node_SendViewEvent : public UK2Node_CallFunction
{
    GENERATED_BODY()

public:
    static const FName EventIdPinName;

    virtual void AllocateDefaultPins() override;
    virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
    virtual FText GetTooltipText() const override;
    virtual FText GetMenuCategory() const override;
    virtual bool CanPasteHere(const UEdGraph* TargetGraph) const override;
    virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
    virtual bool IsActionFilteredOut(const FBlueprintActionFilter& Filter) override;
    virtual void ValidateNodeDuringCompilation(FCompilerResultsLog& MessageLog) const override;

    TArray<FName> GetDeclaredViewEventIds() const;
    bool IsDeclaredEventId(FName EventId) const;
};
