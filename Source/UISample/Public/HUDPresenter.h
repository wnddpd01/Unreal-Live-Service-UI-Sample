#pragma once

#include "CoreMinimal.h"

class UUIPresentationSubsystem;
class UMockPlayerModel;
class UHUDViewModel;

namespace HUDPresentationIds
{
    inline const FName DamageRequested = TEXT("HUD.DamageRequested");
    inline const FName HealRequested = TEXT("HUD.HealRequested");
    inline const FName RefreshViewRequested = TEXT("HUD.RefreshViewRequested");

    inline const FName PlayerModel = TEXT("Model.Player");
    inline const FName View = TEXT("View.HUD");
    inline const FName ViewModel = TEXT("ViewModel.HUD");
}

class UISAMPLE_API FHUDPresenter
{
public:
    static void Install(UUIPresentationSubsystem& Presentation);

private:
    static void RefreshHP(UUIPresentationSubsystem& Presentation, const UMockPlayerModel& Model);
    static void RequestDamage(UUIPresentationSubsystem& Presentation);
    static void RequestHeal(UUIPresentationSubsystem& Presentation);
    static void RefreshView(UUIPresentationSubsystem& Presentation);
};
