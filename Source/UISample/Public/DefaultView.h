#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DefaultView.generated.h"

namespace DefaultViewPresentationIds
{
    inline const FName View = TEXT("View.Default");
    inline const FName Constructed = TEXT("View.Constructed");
};

UCLASS()
class UISAMPLE_API UDefaultView : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    UFUNCTION(BlueprintCallable, Category = "View")
    void EmitViewEvent(FName EventId);
};
