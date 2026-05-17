// Fill out your copyright notice in the Description page of Project Settings.

#include "SamplePlayerController.h"

#include "SampleHUDWidget.h"
#include "Blueprint/UserWidget.h"

void ASamplePlayerController::BeginPlay()
{
    Super::BeginPlay();

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

    HUDWidgetInstance->AddToViewport();

    // 버튼 클릭 테스트를 위해 마우스 커서를 보이게 함
    bShowMouseCursor = true;

    FInputModeGameAndUI InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    SetInputMode(InputMode);
}