// Fill out your copyright notice in the Description page of Project Settings.

#include "HUDViewModel.h"

void UHUDViewModel::SetHPDisplayState(
    int32 InCurrentHP,
    int32 InMaxHP,
    float InHPRatio,
    const FText& InHPText,
    bool bInShowHP
)
{
    CurrentHP = InCurrentHP;
    MaxHP = FMath::Max(InMaxHP, 1);
    HPRatio = FMath::Clamp(InHPRatio, 0.0f, 1.0f);
    HPText = InHPText;
    bShowHP = bInShowHP;

    OnChanged.Broadcast();
}