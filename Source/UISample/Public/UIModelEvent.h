#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UIModelEvent.generated.h"

class UUIModelEvent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUIModelEventRaised, UObject*, Source);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnUIModelEventRaisedNative, UUIModelEvent* /*ModelEvent*/, UObject* /*Source*/);

UCLASS(BlueprintType)
class UISAMPLE_API UUIModelEvent : public UObject
{
    GENERATED_BODY()

public:
    static UUIModelEvent* Create(UObject* InOwner);

    void Initialize(UObject* InOwner);

    UFUNCTION(BlueprintCallable, Category = "UI|Model Event")
    void Raise();

    UFUNCTION(BlueprintPure, Category = "UI|Model Event")
    UObject* GetOwner() const;

    UPROPERTY(BlueprintAssignable, Category = "UI|Model Event")
    FOnUIModelEventRaised OnRaised;

    FOnUIModelEventRaisedNative OnRaisedNative;

protected:
    virtual void BeginDestroy() override;

private:
    void RegisterWithPresentation();
    void UnregisterFromPresentation();

private:
    UPROPERTY(Transient)
    TWeakObjectPtr<UObject> Owner;

    bool bRegisteredWithPresentation = false;
};
