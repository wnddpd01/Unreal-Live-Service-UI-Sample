#include "HUDPresenter.h"

#include "DefaultView.h"
#include "MockPlayerModel.h"
#include "UIPresentationSubsystem.h"
#include "Blueprint/WidgetTree.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void FHUDPresenter::Install(UUIPresentationSubsystem& Presentation)
{
    Presentation.SubscribeModelEvent<UMockPlayerModel>(
        &UMockPlayerModel::HPChanged,
        [&Presentation](UMockPlayerModel& Model)
        {
            FHUDPresenter::RefreshHP(Presentation, Model);
        }
    );

    Presentation.Subscribe(HUDPresentationIds::DamageRequested, [&Presentation]()
        {
            FHUDPresenter::RequestDamage(Presentation);
        });

    Presentation.Subscribe(HUDPresentationIds::HealRequested, [&Presentation]()
        {
            FHUDPresenter::RequestHeal(Presentation);
        });

    Presentation.Subscribe(DefaultViewPresentationIds::Constructed, [&Presentation]()
        {
            FHUDPresenter::RefreshView(Presentation);
        });
}

void FHUDPresenter::RefreshHP(
    UUIPresentationSubsystem& Presentation,
    const UMockPlayerModel& Model
)
{
    UDefaultView* View =
        Presentation.GetObject<UDefaultView>(DefaultViewPresentationIds::View);

    if (!View)
    {
        return;
    }

    UpdateHPView(*View, Model);
}

void FHUDPresenter::UpdateHPView(
    UDefaultView& View,
    const UMockPlayerModel& Model
)
{
    if (!View.WidgetTree)
    {
        return;
    }

    const int32 MaxHP = FMath::Max(Model.GetMaxHP(), 1);
    const int32 CurrentHP = FMath::Clamp(Model.GetCurrentHP(), 0, MaxHP);

    const float HPRatio =
        static_cast<float>(CurrentHP) / static_cast<float>(MaxHP);

    const FText HPText = FText::FromString(
        FString::Printf(TEXT("%d / %d"), CurrentHP, MaxHP)
    );

    if (UTextBlock* HPTextBlock =
        Cast<UTextBlock>(View.WidgetTree->FindWidget(TEXT("HPText"))))
    {
        HPTextBlock->SetText(HPText);
        HPTextBlock->SetVisibility(ESlateVisibility::Visible);
    }

    if (UProgressBar* HPBar =
        Cast<UProgressBar>(View.WidgetTree->FindWidget(TEXT("HPBar"))))
    {
        HPBar->SetPercent(HPRatio);
        HPBar->SetVisibility(ESlateVisibility::Visible);
    }
}

void FHUDPresenter::RequestDamage(UUIPresentationSubsystem& Presentation)
{
    UMockPlayerModel* Model =
        Presentation.GetObject<UMockPlayerModel>(HUDPresentationIds::PlayerModel);

    if (!Model)
    {
        return;
    }

    Model->ApplyDamage(10);
}

void FHUDPresenter::RequestHeal(UUIPresentationSubsystem& Presentation)
{
    UMockPlayerModel* Model =
        Presentation.GetObject<UMockPlayerModel>(HUDPresentationIds::PlayerModel);

    if (!Model)
    {
        return;
    }

    Model->ApplyHeal(10);
}

void FHUDPresenter::RefreshView(UUIPresentationSubsystem& Presentation)
{
    UMockPlayerModel* Model =
        Presentation.GetObject<UMockPlayerModel>(HUDPresentationIds::PlayerModel);

    if (!Model)
    {
        return;
    }

    RefreshHP(Presentation, *Model);
}
