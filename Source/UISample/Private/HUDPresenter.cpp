#include "HUDPresenter.h"

#include "DefaultView.h"
#include "MockPlayerModel.h"
#include "GeneratedUIEventIds.h"
#include "UIMessagePresenterRegistry.h"
#include "Blueprint/WidgetTree.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "PrismUI.h"
#include "PrismUISubsystem.h"

PRISMUI_REGISTER_MESSAGE_PRESENTER(FHUDPresenter)

const TSharedPtr< const FPrismUIBindState > GetHUDBinding()
{
	return UE::PrismUI::FindBindState(FHUDPresenter::PresenterName());
}

void FHUDPresenter::Install(UPrismUISubsystem& Messages)
{
	const FName& PresenterName = FHUDPresenter::PresenterName();

    Messages.Subscribe(PresenterName, UIEvents::Player::HPChanged,
        [](const FUIMessage& Message)
        {
            UMockPlayerModel* Model = Cast<UMockPlayerModel>(Message.GetSource());
            if (!Model)
            {
                return;
            }

            FHUDPresenter::RefreshHP(*Model);
        }
    );

	Messages.Subscribe(PresenterName, UIEvents::Player::HUD::DamageButtonClicked,
		[](const FUIMessage&)
		{
			FHUDPresenter::RequestDamage();
		}
	);

	Messages.Subscribe(PresenterName, UIEvents::Player::HUD::HealButtonClicked,
		[](const FUIMessage&)
		{
			FHUDPresenter::RequestHeal();
		}
	);

	Messages.Subscribe(PresenterName, UIEvents::Player::HUD::Constructed,
		[](const FUIMessage& Message)
		{
			FHUDPresenter::RefreshView(Message);
		}
	);
}

void FHUDPresenter::RefreshHP(const UMockPlayerModel& Model)
{
    const TSharedPtr< const FPrismUIBindState > pBindState = GetHUDBinding();
    if (pBindState == nullptr || pBindState->Model.IsValid() && pBindState->Model.Get() != &Model)
    {
        return;
    }

    UDefaultView* View = Cast<UDefaultView>(pBindState->View.Get());
    if (!View)
    {
        return;
    }

    UpdateHPView( *View, Model);
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
    const TSharedPtr< const FPrismUIBindState > pBindState = GetHUDBinding();
    if (!pBindState)
        return;

    UMockPlayerModel* Model = Cast< UMockPlayerModel >(pBindState->Model.Get());
    if (!Model)
    {
        return;
    }

    Model->ApplyDamage(10);
}

void FHUDPresenter::RequestHeal()
{
    const TSharedPtr< const FPrismUIBindState > pBindState = GetHUDBinding();
    if (!pBindState)
        return;

    UMockPlayerModel* Model = Cast< UMockPlayerModel >(pBindState->Model.Get());
    if (!Model)
    {
        return;
    }

    Model->ApplyHeal(10);
}

void FHUDPresenter::RefreshView(const FUIMessage& Message)
{
    const TSharedPtr< const FPrismUIBindState > pBindState = GetHUDBinding();
    if (!pBindState)
        return;

    UDefaultView* View = Cast<UDefaultView>(Message.GetSource());
    if (pBindState->View.IsValid() && pBindState->View.Get() != View)
    {
        return;
    }

    UMockPlayerModel* Model = Cast< UMockPlayerModel >(pBindState->Model.Get());
    if (!Model)
    {
        return;
    }

    RefreshHP(*Model);
}
