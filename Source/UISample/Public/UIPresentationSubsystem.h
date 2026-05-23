// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "UIPresentationSubsystem.generated.h"


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

private:
    TMap<FName, TArray<TFunction<void()>>> EventHandlers;
    TMap<FName, TWeakObjectPtr<UObject>> ObjectRegistry;
};