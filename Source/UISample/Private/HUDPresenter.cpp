#include "HUDPresenter.h"

#include "HUDViewModel.h"
#include "MockPlayerModel.h"
#include "SampleHUDWidget.h"
#include "UIPresentationSubsystem.h"

void FHUDPresenter::Install(UUIPresentationSubsystem& Presentation)
{
    Presentation.SubscribeModelEvent<UMockPlayerModel>(
        [](UMockPlayerModel& Model)
        {
            return Model.HPChanged.Get();
        },
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

    Presentation.Subscribe(HUDPresentationIds::RefreshViewRequested, [&Presentation]()
        {
            FHUDPresenter::RefreshView(Presentation);
        });
}

void FHUDPresenter::RefreshHP(
    UUIPresentationSubsystem& Presentation,
    const UMockPlayerModel& Model
)
{
    UHUDViewModel* ViewModel =
        Presentation.GetObject<UHUDViewModel>(HUDPresentationIds::ViewModel);

    if (!ViewModel)
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

    ViewModel->SetHPDisplayState(
        CurrentHP,
        MaxHP,
        HPRatio,
        HPText,
        true
    );
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
