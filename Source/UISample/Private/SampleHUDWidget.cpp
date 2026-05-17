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
}

void USampleHUDWidget::SetHP(int32 InCurrentHP, int32 InMaxHP)
{
    const int32 SafeMaxHP = FMath::Max(InMaxHP, 1);
    const int32 SafeCurrentHP = FMath::Clamp(InCurrentHP, 0, SafeMaxHP);

    const float HPRatio = static_cast<float>(SafeCurrentHP) / static_cast<float>(SafeMaxHP);

    if (HPText)
    {
        HPText->SetText(FText::FromString(
            FString::Printf(TEXT("%d / %d"), SafeCurrentHP, SafeMaxHP)
        ));
    }

    if (HPBar)
    {
        HPBar->SetPercent(HPRatio);
    }
}

void USampleHUDWidget::HandleDamageClicked()
{
    OnDamageClicked.Broadcast();
}

void USampleHUDWidget::HandleHealClicked()
{
    OnHealClicked.Broadcast();
}