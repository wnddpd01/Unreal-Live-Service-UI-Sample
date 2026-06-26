#include "SamplePlayerController.h"

#include "DefaultView.h"
#include "GeneratedUIEventIds.h"
#include "MockPlayerModel.h"
#include "UIEventDeclarationMacros.h"
#include "Blueprint/UserWidget.h"
#include "HUDPresenter.h"
#include "PrismUI.h"

void ASamplePlayerController::BeginPlay()
{
    Super::BeginPlay();

    PlayerModel = NewObject<UMockPlayerModel>(this);
    if (!PlayerModel)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to create PlayerModel."));
        return;
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

    FPrismUIBindRequest BindRequest;
    BindRequest.PresenterName = FHUDPresenter::PresenterName();
    BindRequest.Model = PlayerModel;
    BindRequest.View = HUDWidgetInstance;
    UE::PrismUI::Bind(BindRequest);

    HUDWidgetInstance->AddToViewport();

    bShowMouseCursor = true;

    FInputModeGameAndUI InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    SetInputMode(InputMode);

    PlayerModel->Initialize(100);
}
