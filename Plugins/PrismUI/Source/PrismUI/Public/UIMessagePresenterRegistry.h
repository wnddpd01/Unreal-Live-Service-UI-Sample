#pragma once

#include "CoreMinimal.h"

class UPrismUISubsystem;

using FUIMessagePresenterInstallFunction = void (*)(UPrismUISubsystem& Messages);

namespace UIMessagePresenterRegistry
{
    PRISMUI_API void Register(
        FName PresenterId,
        FUIMessagePresenterInstallFunction InstallFunction
    );
    PRISMUI_API void Unregister(FName PresenterId);
    PRISMUI_API void InstallAll(UPrismUISubsystem& Messages);
}

class PRISMUI_API FAutoUIMessagePresenterRegistration
{
public:
    FAutoUIMessagePresenterRegistration(
        FName InPresenterId,
        FUIMessagePresenterInstallFunction InInstallFunction
    );

    ~FAutoUIMessagePresenterRegistration();

private:
    FName PresenterId;
};

#define PRISMUI_PRIVATE_JOIN_INNER(A, B) A##B
#define PRISMUI_PRIVATE_JOIN(A, B) PRISMUI_PRIVATE_JOIN_INNER(A, B)

#define PRISMUI_REGISTER_MESSAGE_PRESENTER(PresenterType) \
    namespace \
    { \
        FAutoUIMessagePresenterRegistration PRISMUI_PRIVATE_JOIN( \
            GAutoUIMessagePresenterRegistration_, \
            __LINE__ \
        )(PresenterType::PresenterId(), &PresenterType::Install); \
    }
