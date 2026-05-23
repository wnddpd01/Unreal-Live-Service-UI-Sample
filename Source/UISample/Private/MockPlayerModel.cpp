#include "MockPlayerModel.h"

#include "Engine/Engine.h"
#include "UIPresentationSubsystem.h"
#include "HUDPresenter.h"

void UMockPlayerModel::Initialize(int32 InMaxHP)
{
    MaxHP = FMath::Max(InMaxHP, 1);
    CurrentHP = MaxHP;

    if (GEngine)
    {
        if (UUIPresentationSubsystem* Presentation =
            GEngine->GetEngineSubsystem<UUIPresentationSubsystem>())
        {
            Presentation->RegisterObject(HUDPresentationIds::PlayerModel, this);
            Presentation->Emit(HUDPresentationIds::HPChanged);
        }
    }
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

    if (GEngine)
    {
        if (UUIPresentationSubsystem* Presentation =
            GEngine->GetEngineSubsystem<UUIPresentationSubsystem>())
        {
            Presentation->Emit(HUDPresentationIds::HPChanged);
        }
    }
}