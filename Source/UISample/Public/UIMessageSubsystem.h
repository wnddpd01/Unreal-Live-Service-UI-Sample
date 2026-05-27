#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "UIMessageSubsystem.generated.h"

USTRUCT(BlueprintType)
struct UISAMPLE_API FUIMessage
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

UCLASS()
class UISAMPLE_API UUIMessageSubsystem : public UEngineSubsystem
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

    void Send(FName EventId, UObject* Source = nullptr);
    void Send(const FUIMessage& Message);

    UFUNCTION(BlueprintCallable, Category = "UI|Messages")
    void SendUIMessage(FName EventId, UObject* Source = nullptr);

public:
    void RegisterObject(FName ObjectId, UObject* Object);

    template <typename T>
    T* GetObject(FName ObjectId) const
    {
        const TWeakObjectPtr<UObject>* FoundObject = ObjectRegistry.Find(ObjectId);
        if (!FoundObject)
        {
            return nullptr;
        }

        return Cast<T>(FoundObject->Get());
    }

public:
    void InstallPresenters();

private:
    TMap<FName, TArray<TFunction<void(const FUIMessage&)>>> MessageHandlers;
    TMap<FName, TWeakObjectPtr<UObject>> ObjectRegistry;
};
