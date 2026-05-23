// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "HUDViewModel.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHUDViewModelChanged);

UCLASS()
class UISAMPLE_API UHUDViewModel : public UObject
{
    GENERATED_BODY()

public:
    void SetHPDisplayState(
        int32 InCurrentHP,
        int32 InMaxHP,
        float InHPRatio,
        const FText& InHPText,
        bool bInShowHP
    );

    int32 GetCurrentHP() const { return CurrentHP; }
    int32 GetMaxHP() const { return MaxHP; }
    float GetHPRatio() const { return HPRatio; }
    const FText& GetHPText() const { return HPText; }
    bool ShouldShowHP() const { return bShowHP; }

    FOnHUDViewModelChanged OnChanged;

private:
    int32 CurrentHP = 0;
    int32 MaxHP = 1;
    float HPRatio = 0.0f;
    FText HPText = FText::GetEmpty();
    bool bShowHP = true;
};