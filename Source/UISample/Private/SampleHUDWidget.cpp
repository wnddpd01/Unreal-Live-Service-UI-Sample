#include "SampleHUDWidget.h"

#include "HUDViewModel.h"
#include "UIPresentationSubsystem.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Engine/Engine.h"
#include "HUDPresenter.h"

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

    UHUDViewModel& HUDViewModel = EnsureViewModel();

    HUDViewModel.OnChanged.RemoveAll(this);
    HUDViewModel.OnChanged.AddUObject(
        this,
        &USampleHUDWidget::RefreshFromViewModel
    );

    if (GEngine)
    {
        if (UUIPresentationSubsystem* Presentation =
            GEngine->GetEngineSubsystem<UUIPresentationSubsystem>())
        {
			Presentation->RegisterObject(HUDPresentationIds::ViewModel, &HUDViewModel);
            Presentation->Emit(HUDPresentationIds::RefreshViewRequested);
        }
    }

    RefreshFromViewModel();
}

UHUDViewModel& USampleHUDWidget::EnsureViewModel()
{
    if (!ViewModel)
    {
        ViewModel = NewObject<UHUDViewModel>(this);
    }

    return *ViewModel;
}

void USampleHUDWidget::RefreshFromViewModel()
{
    if (!ViewModel)
    {
        return;
    }

    if (HPText)
    {
        HPText->SetText(ViewModel->GetHPText());
    }

    if (HPBar)
    {
        HPBar->SetPercent(ViewModel->GetHPRatio());
    }

    const ESlateVisibility HPVisibility =
        ViewModel->ShouldShowHP()
        ? ESlateVisibility::Visible
        : ESlateVisibility::Collapsed;

    if (HPText)
    {
        HPText->SetVisibility(HPVisibility);
    }

    if (HPBar)
    {
        HPBar->SetVisibility(HPVisibility);
    }
}

void USampleHUDWidget::HandleDamageClicked()
{
    if (GEngine)
    {
        if (UUIPresentationSubsystem* Presentation =
            GEngine->GetEngineSubsystem<UUIPresentationSubsystem>())
        {
            Presentation->Emit(HUDPresentationIds::DamageRequested);
        }
    }
}

void USampleHUDWidget::HandleHealClicked()
{
    if (GEngine)
    {
        if (UUIPresentationSubsystem* Presentation =
            GEngine->GetEngineSubsystem<UUIPresentationSubsystem>())
        {
            Presentation->Emit(HUDPresentationIds::HealRequested);
        }
    }
}