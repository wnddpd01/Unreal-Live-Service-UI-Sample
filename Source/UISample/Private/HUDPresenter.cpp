#include "HUDPresenter.h"

#include "DefaultView.h"
#include "MockPlayerModel.h"
#include "GeneratedUIEventIds.h"
#include "UIMessagePresenterRegistry.h"
#include "PrismUISubsystem.h"
#include "Blueprint/WidgetTree.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

PRISMUI_REGISTER_MESSAGE_PRESENTER(FHUDPresenter)

namespace
{
    struct FHUDBinding
    {
        TWeakObjectPtr<UMockPlayerModel> Model;
        TWeakObjectPtr<UDefaultView> View;
    };

    FHUDBinding& GetHUDBinding()
    {
        static FHUDBinding Binding;
        return Binding;
    }
}

void FHUDPresenter::Install(UPrismUISubsystem& Messages)
{
    static TWeakObjectPtr<UPrismUISubsystem> InstalledMessages;
    if (InstalledMessages.Get() == &Messages)
    {
        return;
    }

    InstalledMessages = &Messages;

    Messages.Subscribe(UIEvents::Player::HUD::ConnectRequested, [](const FUIMessage& Message)
        {
            FHUDPresenter::Connect(Message);
        });

    Messages.Subscribe(UIEvents::Player::HPChanged, [](const FUIMessage& Message)
        {
            UMockPlayerModel* Model = Cast<UMockPlayerModel>(Message.GetSource());
            if (!Model)
            {
                return;
            }

            FHUDPresenter::RefreshHP(*Model);
        }
    );

    Messages.Subscribe(UIEvents::Player::HUD::DamageButtonClicked, [](const FUIMessage&)
        {
            FHUDPresenter::RequestDamage();
        });

    Messages.Subscribe(UIEvents::Player::HUD::HealButtonClicked, [](const FUIMessage&)
        {
            FHUDPresenter::RequestHeal();
        });

    Messages.Subscribe(UIEvents::Player::HUD::Constructed, [](const FUIMessage& Message)
        {
            FHUDPresenter::RefreshView(Message);
        });
}

void FHUDPresenter::Connect(const FUIMessage& Message)
{
    UMockPlayerModel* Model = Cast<UMockPlayerModel>(Message.GetSource());
    UDefaultView* View = Cast<UDefaultView>(Message.GetTarget());

    if (!Model || !View)
    {
        return;
    }

    FHUDBinding& Binding = GetHUDBinding();
    Binding.Model = Model;
    Binding.View = View;

    UpdateHPView(*View, *Model);
}

void FHUDPresenter::RefreshHP(const UMockPlayerModel& Model)
{
    FHUDBinding& Binding = GetHUDBinding();
    if (Binding.Model.IsValid() && Binding.Model.Get() != &Model)
    {
        return;
    }

    UDefaultView* View = Binding.View.Get();
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

void FHUDPresenter::RequestDamage()
{
    UMockPlayerModel* Model = GetHUDBinding().Model.Get();
    if (!Model)
    {
        return;
    }

    Model->ApplyDamage(10);
}

void FHUDPresenter::RequestHeal()
{
    UMockPlayerModel* Model = GetHUDBinding().Model.Get();
    if (!Model)
    {
        return;
    }

    Model->ApplyHeal(10);
}

void FHUDPresenter::RefreshView(const FUIMessage& Message)
{
    FHUDBinding& Binding = GetHUDBinding();
    UDefaultView* View = Cast<UDefaultView>(Message.GetSource());
    if (Binding.View.IsValid() && Binding.View.Get() != View)
    {
        return;
    }

    UMockPlayerModel* Model = Binding.Model.Get();
    if (!Model)
    {
        return;
    }

    RefreshHP(*Model);
}
