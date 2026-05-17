// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MockPlayerModel.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnMockPlayerHPChanged, int32 /*CurrentHP*/, int32 /*MaxHP*/);

UCLASS()
class UISAMPLE_API UMockPlayerModel : public UObject
{
    GENERATED_BODY()

public:
    void Initialize(int32 InMaxHP);

    void ApplyDamage(int32 DamageAmount);
    void ApplyHeal(int32 HealAmount);

    int32 GetCurrentHP() const { return CurrentHP; }
    int32 GetMaxHP() const { return MaxHP; }

    FOnMockPlayerHPChanged OnHPChanged;

private:
    void SetHP(int32 NewHP);
    void BroadcastHPChanged();

private:
    int32 CurrentHP = 100;
    int32 MaxHP = 100;
};