#pragma once

#include "CoreMinimal.h"
#include "PrismUIPresenterId.h"

class UPrismUISubsystem;
struct FUIMessage;
class UMockPlayerModel;
class UDefaultView;

class UISAMPLE_API FHUDPresenter
{
    PRISMUI_PRESENTER_ID(FHUDPresenter);

public:
    static void Install(UPrismUISubsystem& Messages);

private:
    static void RefreshHP(const UMockPlayerModel& Model);
    static void UpdateHPView(UDefaultView& View, const UMockPlayerModel& Model);
    static void RequestDamage();
    static void RequestHeal();
    static void RefreshView(const FUIMessage& Message);
};
