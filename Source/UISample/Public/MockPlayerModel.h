// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UIEventDeclarationMacros.h"
#include "UObject/Object.h"
#include "MockPlayerModel.generated.h"

UCLASS()
class UISAMPLE_API UMockPlayerModel : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Mock Player")
    void Initialize(int32 InMaxHP);

    UFUNCTION(BlueprintCallable, Category = "Mock Player")
    void ApplyDamage(int32 DamageAmount);

    UFUNCTION(BlueprintCallable, Category = "Mock Player")
    void ApplyHeal(int32 HealAmount);

    UFUNCTION(BlueprintPure, Category = "Mock Player")
    int32 GetCurrentHP() const { return CurrentHP; }

    UFUNCTION(BlueprintPure, Category = "Mock Player")
    int32 GetMaxHP() const { return MaxHP; }

    PRISMUI_EVENT_DECLARE(Player.HPChanged)

private:
    void SetHP(int32 NewHP);
    void BroadcastHPChanged();

private:
    int32 CurrentHP = 100;
    int32 MaxHP = 100;
};
