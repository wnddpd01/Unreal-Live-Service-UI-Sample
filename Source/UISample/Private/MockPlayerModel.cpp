#include "MockPlayerModel.h"

#include "GeneratedUIEventIds.h"
#include "UIMessageSubsystem.h"
#include "Engine/Engine.h"

void UMockPlayerModel::Initialize(int32 InMaxHP)
{
    MaxHP = FMath::Max(InMaxHP, 1);
    CurrentHP = MaxHP;

    BroadcastHPChanged();
}

void UMockPlayerModel::ApplyDamage(int32 DamageAmount)
{
    SetHP(CurrentHP - FMath::Max(DamageAmount, 0));
}

void UMockPlayerModel::ApplyHeal(int32 HealAmount)
{
    SetHP(CurrentHP + FMath::Max(HealAmount, 0));
}

void UMockPlayerModel::SetHP(int32 NewHP)
{
    const int32 ClampedHP = FMath::Clamp(NewHP, 0, MaxHP);

    if (CurrentHP == ClampedHP)
    {
        return;
    }

    CurrentHP = ClampedHP;

    BroadcastHPChanged();
}

void UMockPlayerModel::BroadcastHPChanged()
{
    if (!GEngine)
    {
        return;
    }

    if (UUIMessageSubsystem* Messages =
        GEngine->GetEngineSubsystem<UUIMessageSubsystem>())
    {
        Messages->Send(UIEvents::Player::HPChanged, this);
    }
}
