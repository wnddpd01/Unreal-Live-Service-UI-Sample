#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "CollectUIEventsCommandlet.generated.h"

UCLASS()
class UISAMPLEEDITOR_API UCollectUIEventsCommandlet : public UCommandlet
{
    GENERATED_BODY()

public:
    UCollectUIEventsCommandlet();

    virtual int32 Main(const FString& Params) override;
};
