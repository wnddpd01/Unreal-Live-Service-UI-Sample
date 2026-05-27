#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "RefreshUIEventContractsCommandlet.generated.h"

UCLASS()
class PRISMUIEDITOR_API URefreshUIEventContractsCommandlet : public UCommandlet
{
    GENERATED_BODY()

public:
    URefreshUIEventContractsCommandlet();

    virtual int32 Main(const FString& Params) override;
};
