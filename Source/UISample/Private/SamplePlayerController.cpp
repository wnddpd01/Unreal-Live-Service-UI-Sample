// Fill out your copyright notice in the Description page of Project Settings.

#include "SamplePlayerController.h"

#include "MockPlayerModel.h"
#include "SampleHUDWidget.h"
#include "Blueprint/UserWidget.h"

void ASamplePlayerController::BeginPlay()
{
    Super::BeginPlay();

    PlayerModel = NewObject<UMockPlayerModel>(this);
    if (!PlayerModel)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to create PlayerModel."));
        return;
    }

    PlayerModel->OnHPChanged.AddUObject(this, &ASamplePlayerController::HandleHPChanged);
    PlayerModel->Initialize(100);

    if (!HUDWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("HUDWidgetClass is not set."));
        return;
    }

    HUDWidgetInstance = CreateWidget<USampleHUDWidget>(this, HUDWidgetClass);
    if (!HUDWidgetInstance)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to create HUDWidgetInstance."));
        return;
    }

    HUDWidgetInstance->OnDamageClicked.AddUObject(this, &ASamplePlayerController::HandleDamageClicked);
    HUDWidgetInstance->OnHealClicked.AddUObject(this, &ASamplePlayerController::HandleHealClicked);

    HUDWidgetInstance->AddToViewport();

    // 초기 HP 표시
    HUDWidgetInstance->SetHP(PlayerModel->GetCurrentHP(), PlayerModel->GetMaxHP());

    bShowMouseCursor = true;

    FInputModeGameAndUI InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    SetInputMode(InputMode);
}

void ASamplePlayerController::HandleDamageClicked()
{
    if (PlayerModel)
    {
        PlayerModel->ApplyDamage(10);
    }
}

void ASamplePlayerController::HandleHealClicked()
{
    if (PlayerModel)
    {
        PlayerModel->ApplyHeal(10);
    }
}

void ASamplePlayerController::HandleHPChanged(int32 CurrentHP, int32 MaxHP)
{
    if (HUDWidgetInstance)
    {
        HUDWidgetInstance->SetHP(CurrentHP, MaxHP);
    }
}