#include "UIMessagePresenterRegistry.h"

#include "Engine/Engine.h"
#include "UIMessageSubsystem.h"

namespace
{
    struct FRegisteredUIMessagePresenter
    {
        FName PresenterId;
        FUIMessagePresenterInstallFunction InstallFunction = nullptr;
    };

    TArray<FRegisteredUIMessagePresenter>& GetRegisteredPresenters()
    {
        static TArray<FRegisteredUIMessagePresenter>* Presenters =
            new TArray<FRegisteredUIMessagePresenter>();
        return *Presenters;
    }
}

void UIMessagePresenterRegistry::Register(
    FName PresenterId,
    FUIMessagePresenterInstallFunction InstallFunction
)
{
    if (PresenterId.IsNone() || !InstallFunction)
    {
        return;
    }

    TArray<FRegisteredUIMessagePresenter>& Presenters = GetRegisteredPresenters();
    if (FRegisteredUIMessagePresenter* ExistingPresenter =
        Presenters.FindByPredicate([PresenterId](const FRegisteredUIMessagePresenter& Presenter)
            {
                return Presenter.PresenterId == PresenterId;
            }))
    {
        ExistingPresenter->InstallFunction = InstallFunction;
    }
    else
    {
        Presenters.Add({ PresenterId, InstallFunction });
    }

    if (GEngine)
    {
        if (UUIMessageSubsystem* Messages =
            GEngine->GetEngineSubsystem<UUIMessageSubsystem>())
        {
            InstallFunction(*Messages);
        }
    }
}

void UIMessagePresenterRegistry::Unregister(FName PresenterId)
{
    if (PresenterId.IsNone())
    {
        return;
    }

    GetRegisteredPresenters().RemoveAll(
        [PresenterId](const FRegisteredUIMessagePresenter& Presenter)
        {
            return Presenter.PresenterId == PresenterId;
        }
    );
}

void UIMessagePresenterRegistry::InstallAll(UUIMessageSubsystem& Messages)
{
    const TArray<FRegisteredUIMessagePresenter> Presenters = GetRegisteredPresenters();
    for (const FRegisteredUIMessagePresenter& Presenter : Presenters)
    {
        if (Presenter.InstallFunction)
        {
            Presenter.InstallFunction(Messages);
        }
    }
}

FAutoUIMessagePresenterRegistration::FAutoUIMessagePresenterRegistration(
    FName InPresenterId,
    FUIMessagePresenterInstallFunction InInstallFunction
)
    : PresenterId(InPresenterId)
{
    UIMessagePresenterRegistry::Register(PresenterId, InInstallFunction);
}

FAutoUIMessagePresenterRegistration::~FAutoUIMessagePresenterRegistration()
{
    UIMessagePresenterRegistry::Unregister(PresenterId);
}
