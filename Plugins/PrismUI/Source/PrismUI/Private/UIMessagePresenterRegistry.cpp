#include "UIMessagePresenterRegistry.h"

#include "Engine/Engine.h"
#include "PrismUISubsystem.h"

namespace
{
    struct FRegisteredUIMessagePresenter
    {
        FName PresenterName;
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
    FName PresenterName,
    FUIMessagePresenterInstallFunction InstallFunction
)
{
    if (PresenterName.IsNone() || !InstallFunction)
    {
        return;
    }

    TArray<FRegisteredUIMessagePresenter>& Presenters = GetRegisteredPresenters();
    if (FRegisteredUIMessagePresenter* ExistingPresenter =
        Presenters.FindByPredicate([PresenterName](const FRegisteredUIMessagePresenter& Presenter)
            {
                return Presenter.PresenterName == PresenterName;
            }))
    {
        ExistingPresenter->InstallFunction = InstallFunction;
    }
    else
    {
        Presenters.Add({ PresenterName, InstallFunction });
    }

    if (GEngine)
    {
        if (UPrismUISubsystem* Messages =
            GEngine->GetEngineSubsystem<UPrismUISubsystem>())
        {
            InstallFunction(*Messages);
        }
    }
}

void UIMessagePresenterRegistry::Unregister(FName PresenterName)
{
    if (PresenterName.IsNone())
    {
        return;
    }

    GetRegisteredPresenters().RemoveAll(
        [PresenterName](const FRegisteredUIMessagePresenter& Presenter)
        {
            return Presenter.PresenterName == PresenterName;
        }
    );
}

void UIMessagePresenterRegistry::InstallAll(UPrismUISubsystem& Messages)
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
    FName InPresenterName,
    FUIMessagePresenterInstallFunction InInstallFunction
)
    : PresenterName(InPresenterName)
{
    UIMessagePresenterRegistry::Register(PresenterName, InInstallFunction);
}

FAutoUIMessagePresenterRegistration::~FAutoUIMessagePresenterRegistration()
{
    UIMessagePresenterRegistry::Unregister(PresenterName);
}
