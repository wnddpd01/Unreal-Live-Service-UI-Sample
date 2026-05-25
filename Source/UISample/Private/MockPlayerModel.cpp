#include "MockPlayerModel.h"

void UMockPlayerModel::Initialize(int32 InMaxHP)
{
    EnsureModelEvents();

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

void UMockPlayerModel::EnsureModelEvents()
{
    if (!HPChanged)
    {
        HPChanged = UUIModelEvent::Create(this);
    }
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
    EnsureModelEvents();

    if (HPChanged)
    {
        HPChanged->Raise();
    }
}
