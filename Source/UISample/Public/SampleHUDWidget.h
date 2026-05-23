#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Button.h"
#include "SampleHUDWidget.generated.h"

class UHUDViewModel;

DECLARE_MULTICAST_DELEGATE(FOnHUDButtonClicked);

UCLASS()
class UISAMPLE_API USampleHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    UHUDViewModel& EnsureViewModel();
    void RefreshFromViewModel();

private:
    UFUNCTION()
    void HandleDamageClicked();

    UFUNCTION()
    void HandleHealClicked();

private:
    UPROPERTY(Transient)
    TObjectPtr<UHUDViewModel> ViewModel;

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> HPText = nullptr;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> HPBar = nullptr;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> DamageButton = nullptr;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> HealButton = nullptr;
};