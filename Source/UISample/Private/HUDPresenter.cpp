#include "HUDPresenter.h"

#include "DefaultView.h"
#include "MockPlayerModel.h"
#include "GeneratedUIEventIds.h"
#include "UIMessageSubsystem.h"
#include "Blueprint/WidgetTree.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void FHUDPresenter::Install(UUIMessageSubsystem& Messages)
{
    static TWeakObjectPtr<UUIMessageSubsystem> InstalledMessages;
    if (InstalledMessages.Get() == &Messages)
    {
        return;
    }

    InstalledMessages = &Messages;

    Messages.Subscribe(UIEvents::Model::Player::HPChanged, [&Messages](const FUIMessage& Message)
        {
            UMockPlayerModel* Model = Cast<UMockPlayerModel>(Message.GetSource());
            if (!Model)
            {
                return;
            }

            FHUDPresenter::RefreshHP(Messages, *Model);
        }
    );

    Messages.Subscribe(UIEvents::View::HUD::DamageButtonClicked, [&Messages](const FUIMessage& Message)
        {
            FHUDPresenter::RequestDamage(Messages);
        });

    Messages.Subscribe(UIEvents::View::HUD::HealButtonClicked, [&Messages](const FUIMessage& Message)
        {
            FHUDPresenter::RequestHeal(Messages);
        });

    Messages.Subscribe(UIEvents::View::HUD::Constructed, [&Messages](const FUIMessage& Message)
        {
            FHUDPresenter::RefreshView(Messages);
        });
}

void FHUDPresenter::RefreshHP(
    UUIMessageSubsystem& Messages,
    const UMockPlayerModel& Model
)
{
    UDefaultView* View =
        Messages.GetObject<UDefaultView>(DefaultViewPresentationIds::View);

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

void FHUDPresenter::RequestDamage(UUIMessageSubsystem& Messages)
{
    UMockPlayerModel* Model =
        Messages.GetObject<UMockPlayerModel>(HUDPresentationIds::PlayerModel);

    if (!Model)
    {
        return;
    }

    Model->ApplyDamage(10);
}

void FHUDPresenter::RequestHeal(UUIMessageSubsystem& Messages)
{
    UMockPlayerModel* Model =
        Messages.GetObject<UMockPlayerModel>(HUDPresentationIds::PlayerModel);

    if (!Model)
    {
        return;
    }

    Model->ApplyHeal(10);
}

void FHUDPresenter::RefreshView(UUIMessageSubsystem& Messages)
{
    UMockPlayerModel* Model =
        Messages.GetObject<UMockPlayerModel>(HUDPresentationIds::PlayerModel);

    if (!Model)
    {
        return;
    }

    RefreshHP(Messages, *Model);
}
