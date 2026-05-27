#include "SamplePlayerController.h"

#include "DefaultView.h"
#include "MockPlayerModel.h"
#include "UIPresentationSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"
#include "HUDPresenter.h"

void ASamplePlayerController::BeginPlay()
{
    Super::BeginPlay();

    PlayerModel = NewObject<UMockPlayerModel>(this);
    if (!PlayerModel)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to create PlayerModel."));
        return;
    }

    if (GEngine)
    {
        if (UUIPresentationSubsystem* Presentation =
            GEngine->GetEngineSubsystem<UUIPresentationSubsystem>())
        {
            Presentation->RegisterObject(HUDPresentationIds::PlayerModel, PlayerModel);
        }
    }

    if (!HUDWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("HUDWidgetClass is not set."));
        return;
    }

    HUDWidgetInstance = CreateWidget<UDefaultView>(this, HUDWidgetClass);
    if (!HUDWidgetInstance)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to create HUDWidgetInstance."));
        return;
    }

    HUDWidgetInstance->AddToViewport();

    bShowMouseCursor = true;

    FInputModeGameAndUI InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    SetInputMode(InputMode);

    PlayerModel->Initialize(100);
}
