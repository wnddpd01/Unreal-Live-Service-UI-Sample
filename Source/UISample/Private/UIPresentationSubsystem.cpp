// Fill out your copyright notice in the Description page of Project Settings.

#include "UIPresentationSubsystem.h"

#include "HUDPresenter.h"

void UUIPresentationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    InstallPresenters();

    UE_LOG(LogTemp, Log, TEXT("UIPresentationSubsystem initialized."));
}

void UUIPresentationSubsystem::Deinitialize()
{
    EventHandlers.Reset();
    ObjectRegistry.Reset();

    UE_LOG(LogTemp, Log, TEXT("UIPresentationSubsystem deinitialized."));

    Super::Deinitialize();
}

void UUIPresentationSubsystem::Emit(FName EventId)
{
    TArray<TFunction<void()>>* Handlers = EventHandlers.Find(EventId);
    if (!Handlers)
    {
        return;
    }

    for (TFunction<void()>& Handler : *Handlers)
    {
        if (Handler)
        {
            Handler();
        }
    }
}

void UUIPresentationSubsystem::RegisterObject(FName ObjectId, UObject* Object)
{
    if (!Object)
    {
        ObjectRegistry.Remove(ObjectId);
        return;
    }

    ObjectRegistry.Add(ObjectId, Object);
}

void UUIPresentationSubsystem::InstallPresenters()
{
    FHUDPresenter::Install(*this);
}