#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DefaultView.generated.h"

namespace DefaultViewPresentationIds
{
    inline const FName View = TEXT("View.Default");
};

UCLASS()
class PRISMUI_API UDefaultView : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    UFUNCTION(BlueprintCallable, Category = "View", meta = (BlueprintInternalUseOnly = "true"))
    void SendViewEvent(FName EventId);

    FName GetViewId() const { return ViewId; }
    const TArray<FName>& GetDeclaredViewEvents() const { return DeclaredViewEvents; }

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "View Events")
    FName ViewId = TEXT("HUD");

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "View Events")
    TArray<FName> DeclaredViewEvents;

private:
    FName MakeViewEventId(FName EventName) const;
    FName NormalizeViewEventId(FName EventId) const;
    bool IsDeclaredViewEvent(FName EventId) const;
};
