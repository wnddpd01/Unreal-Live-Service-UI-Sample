// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "UIPresentationSubsystem.generated.h"

class UUIModelEvent;

struct FUIModelEventSubscription
{
    UClass* ModelClass = nullptr;
    TFunction<UUIModelEvent*(UObject*)> ResolveEvent;
    TFunction<void(UObject*)> HandleEvent;
};

UCLASS()
class UISAMPLE_API UUIPresentationSubsystem : public UEngineSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

public:
    template <typename LambdaType>
    void Subscribe(FName EventId, LambdaType&& Lambda)
    {
        TFunction<void()> Handler(Forward<LambdaType>(Lambda));
        EventHandlers.FindOrAdd(EventId).Add(MoveTemp(Handler));
    }

    void Emit(FName EventId);

public:
    void RegisterModelEvent(UUIModelEvent* ModelEvent);
    void UnregisterModelEvent(UUIModelEvent* ModelEvent);

    template <typename ModelType, typename EventAccessorType, typename HandlerType>
    void SubscribeModelEvent(EventAccessorType&& EventAccessor, HandlerType&& Handler)
    {
        FUIModelEventSubscription Subscription;
        Subscription.ModelClass = ModelType::StaticClass();
        Subscription.ResolveEvent =
            [EventAccessor = Forward<EventAccessorType>(EventAccessor)](UObject* Source) -> UUIModelEvent*
            {
                ModelType* Model = Cast<ModelType>(Source);
                if (!Model)
                {
                    return nullptr;
                }

                return EventAccessor(*Model);
            };
        Subscription.HandleEvent =
            [Handler = Forward<HandlerType>(Handler)](UObject* Source)
            {
                ModelType* Model = Cast<ModelType>(Source);
                if (Model)
                {
                    Handler(*Model);
                }
            };

        ModelEventSubscriptions.Add(MoveTemp(Subscription));
    }

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

private:
    void InstallPresenters();
    void HandleModelEventRaised(UUIModelEvent* ModelEvent, UObject* Source);

private:
    TMap<FName, TArray<TFunction<void()>>> EventHandlers;
    TMap<FName, TWeakObjectPtr<UObject>> ObjectRegistry;
    TMap<UUIModelEvent*, FDelegateHandle> ModelEventHandles;
    TArray<FUIModelEventSubscription> ModelEventSubscriptions;
};
