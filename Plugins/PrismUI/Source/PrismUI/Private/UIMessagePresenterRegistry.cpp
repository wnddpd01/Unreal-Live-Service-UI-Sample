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

    void InstallPresenter(
        UPrismUISubsystem& Messages,
        const FRegisteredUIMessagePresenter& Presenter
    )
    {
        if (Presenter.PresenterName.IsNone())
        {
            return;
        }

        Messages.ClearPresenterSubscriptions(Presenter.PresenterName);

        if (Presenter.InstallFunction)
        {
            Presenter.InstallFunction(Messages);
        }
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
    FRegisteredUIMessagePresenter* RegisteredPresenter =
        Presenters.FindByPredicate([PresenterName](const FRegisteredUIMessagePresenter& Presenter)
            {
                return Presenter.PresenterName == PresenterName;
            });

    if (RegisteredPresenter)
    {
        RegisteredPresenter->InstallFunction = InstallFunction;
    }
    else
    {
        RegisteredPresenter = &Presenters.Add_GetRef({ PresenterName, InstallFunction });
    }

    if (GEngine)
    {
        if (UPrismUISubsystem* Messages =
            GEngine->GetEngineSubsystem<UPrismUISubsystem>())
        {
            InstallPresenter(*Messages, *RegisteredPresenter);
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

    if (GEngine)
    {
        if (UPrismUISubsystem* Messages =
            GEngine->GetEngineSubsystem<UPrismUISubsystem>())
        {
            Messages->ClearPresenterSubscriptions(PresenterName);
        }
    }
}

void UIMessagePresenterRegistry::InstallAll(UPrismUISubsystem& Messages)
{
    const TArray<FRegisteredUIMessagePresenter> Presenters = GetRegisteredPresenters();
    for (const FRegisteredUIMessagePresenter& Presenter : Presenters)
    {
        InstallPresenter(Messages, Presenter);
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
