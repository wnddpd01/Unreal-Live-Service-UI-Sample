#pragma once

#include "CoreMinimal.h"

class UPrismUISubsystem;
struct FUIMessage;
class UMockPlayerModel;
class UDefaultView;

class UISAMPLE_API FHUDPresenter
{
public:
    static void Install(UPrismUISubsystem& Messages);

private:
    static void Connect(const FUIMessage& Message);
    static void RefreshHP(const UMockPlayerModel& Model);
    static void UpdateHPView(UDefaultView& View, const UMockPlayerModel& Model);
    static void RequestDamage();
    static void RequestHeal();
    static void RefreshView(const FUIMessage& Message);
};
