// Fill out your copyright notice in the Description page of Project Settings.
#include "SampleHUDWidget.h"

void USampleHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (DamageButton)
    {
        DamageButton->OnClicked.AddDynamic(this, &USampleHUDWidget::HandleDamageClicked);
    }

    if (HealButton)
    {
        HealButton->OnClicked.AddDynamic(this, &USampleHUDWidget::HandleHealClicked);
    }

    SetHP(CurrentHP, MaxHP);
}

void USampleHUDWidget::SetHP(int32 InCurrentHP, int32 InMaxHP)
{
    MaxHP = FMath::Max(InMaxHP, 1);
    CurrentHP = FMath::Clamp(InCurrentHP, 0, MaxHP);

    const float HPRatio = static_cast<float>(CurrentHP) / static_cast<float>(MaxHP);

    if (HPText)
    {
        HPText->SetText(FText::FromString(
            FString::Printf(TEXT("%d / %d"), CurrentHP, MaxHP)
        ));
    }

    if (HPBar)
    {
        HPBar->SetPercent(HPRatio);
    }
}

void USampleHUDWidget::HandleDamageClicked()
{
    SetHP(CurrentHP - 10, MaxHP);
}

void USampleHUDWidget::HandleHealClicked()
{
    SetHP(CurrentHP + 10, MaxHP);
}