// Fill out your copyright notice in the Description page of Project Settings.

#include "UIPresentationSubsystem.h"

#include "HUDPresenter.h"
#include "UIModelEvent.h"

void UUIPresentationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    InstallPresenters();

    UE_LOG(LogTemp, Log, TEXT("UIPresentationSubsystem initialized."));
}

void UUIPresentationSubsystem::Deinitialize()
{
    for (TPair<UUIModelEvent*, FDelegateHandle>& Pair : ModelEventHandles)
    {
        if (Pair.Key)
        {
            Pair.Key->OnRaisedNative.Remove(Pair.Value);
        }
    }

    EventHandlers.Reset();
    ObjectRegistry.Reset();
    ModelEventHandles.Reset();
    ModelEventSubscriptions.Reset();

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

void UUIPresentationSubsystem::RegisterModelEvent(UUIModelEvent* ModelEvent)
{
    if (!ModelEvent || ModelEventHandles.Contains(ModelEvent))
    {
        return;
    }

    const FDelegateHandle Handle = ModelEvent->OnRaisedNative.AddUObject(
        this,
        &UUIPresentationSubsystem::HandleModelEventRaised
    );

    ModelEventHandles.Add(ModelEvent, Handle);
}

void UUIPresentationSubsystem::UnregisterModelEvent(UUIModelEvent* ModelEvent)
{
    if (!ModelEvent)
    {
        return;
    }

    FDelegateHandle* FoundHandle = ModelEventHandles.Find(ModelEvent);
    if (!FoundHandle)
    {
        return;
    }

    ModelEvent->OnRaisedNative.Remove(*FoundHandle);
    ModelEventHandles.Remove(ModelEvent);
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

void UUIPresentationSubsystem::HandleModelEventRaised(
    UUIModelEvent* ModelEvent,
    UObject* Source
)
{
    if (!ModelEvent || !Source)
    {
        return;
    }

    for (const FUIModelEventSubscription& Subscription : ModelEventSubscriptions)
    {
        if (!Subscription.ModelClass ||
            !Source->IsA(Subscription.ModelClass) ||
            !Subscription.ResolveEvent ||
            !Subscription.HandleEvent)
        {
            continue;
        }

        if (Subscription.ResolveEvent(Source) == ModelEvent)
        {
            Subscription.HandleEvent(Source);
        }
    }
}

void UUIPresentationSubsystem::InstallPresenters()
{
    FHUDPresenter::Install(*this);
}
