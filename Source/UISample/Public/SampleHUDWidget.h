#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Button.h"
#include "SampleHUDWidget.generated.h"

UCLASS()
class UISAMPLE_API USampleHUDWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

public:
    UFUNCTION(BlueprintCallable)
    void SetHP(int32 InCurrentHP, int32 InMaxHP);

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> HPText = nullptr;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> HPBar = nullptr;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> DamageButton = nullptr;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> HealButton = nullptr;

private:
    UFUNCTION()
    void HandleDamageClicked();

    UFUNCTION()
    void HandleHealClicked();

private:
    int32 CurrentHP = 100;
    int32 MaxHP = 100;
};