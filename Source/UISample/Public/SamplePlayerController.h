// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SamplePlayerController.generated.h"

class UDefaultView;
class UMockPlayerModel;

UCLASS()
class UISAMPLE_API ASamplePlayerController : public APlayerController
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UDefaultView> HUDWidgetClass;

private:
    void HandleDamageClicked();
    void HandleHealClicked();
    void HandleHPChanged(int32 CurrentHP, int32 MaxHP);

private:
    UPROPERTY(Transient)
    TObjectPtr<UDefaultView> HUDWidgetInstance;

    UPROPERTY(Transient)
    TObjectPtr<UMockPlayerModel> PlayerModel;
};
