#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "UIMessageSubsystem.generated.h"

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

    TWeakObjectPtr<UObject> Target;

    UObject* GetTarget() const
    {
        return Target.Get();
    }
};

UCLASS()
class PRISMUI_API UUIMessageSubsystem : public UEngineSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

public:
    template <typename HandlerType>
    void Subscribe(FName EventId, HandlerType&& Handler)
    {
        TFunction<void(const FUIMessage&)> WrappedHandler(Forward<HandlerType>(Handler));
        MessageHandlers.FindOrAdd(EventId).Add(MoveTemp(WrappedHandler));
    }

    void Send(FName EventId, UObject* Source = nullptr, UObject* Target = nullptr);
    void Send(const FUIMessage& Message);

    UFUNCTION(BlueprintCallable, Category = "UI|Messages")
    void SendUIMessage(FName EventId, UObject* Source = nullptr, UObject* Target = nullptr);

private:
    TMap<FName, TArray<TFunction<void(const FUIMessage&)>>> MessageHandlers;
};
