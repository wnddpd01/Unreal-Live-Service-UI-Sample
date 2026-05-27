#pragma once

#include "CoreMinimal.h"

class UUIMessageSubsystem;
struct FUIMessage;
class UMockPlayerModel;
class UDefaultView;

namespace HUDPresentationIds
{
    inline const FName PlayerModel = TEXT("Model.Player");
}

class UISAMPLE_API FHUDPresenter
{
public:
    static void Install(UUIMessageSubsystem& Messages);

private:
    static void RefreshHP(UUIMessageSubsystem& Messages, const UMockPlayerModel& Model);
    static void UpdateHPView(UDefaultView& View, const UMockPlayerModel& Model);
    static void RequestDamage(UUIMessageSubsystem& Messages);
    static void RequestHeal(UUIMessageSubsystem& Messages);
    static void RefreshView(UUIMessageSubsystem& Messages);
};
