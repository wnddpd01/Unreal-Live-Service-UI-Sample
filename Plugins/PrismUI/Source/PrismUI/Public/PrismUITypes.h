#pragma once

#include "CoreMinimal.h"
#include "PrismUITypes.generated.h"

USTRUCT(BlueprintType)
struct PRISMUI_API FUIMessage
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "UI Message")
    FName EventId;

    TWeakObjectPtr<UObject> Source;

    UObject* GetSource() const
    {
        return Source.Get();
    }
};

USTRUCT(BlueprintType)
struct PRISMUI_API FPrismUIBindRequest
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName BindingId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName PresenterName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UObject> Model = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UObject> View = nullptr;
};

struct PRISMUI_API FPrismUIBindState
{
    FName BindingId;
    FName PresenterName;
    TWeakObjectPtr<UObject> Model;
    TWeakObjectPtr<UObject> View;

    bool IsValid() const
    {
        return Model.IsValid() && View.IsValid();
    }
};
