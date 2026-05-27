#include "UIMessageSubsystem.h"

void UUIMessageSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    UE_LOG(LogTemp, Log, TEXT("UIMessageSubsystem initialized."));
}

void UUIMessageSubsystem::Deinitialize()
{
    MessageHandlers.Reset();
    ObjectRegistry.Reset();

    UE_LOG(LogTemp, Log, TEXT("UIMessageSubsystem deinitialized."));

    Super::Deinitialize();
}

void UUIMessageSubsystem::Send(FName EventId, UObject* Source)
{
    FUIMessage Message;
    Message.EventId = EventId;
    Message.Source = Source;

    Send(Message);
}

void UUIMessageSubsystem::Send(const FUIMessage& Message)
{
    if (Message.EventId.IsNone())
    {
        return;
    }

    TArray<TFunction<void(const FUIMessage&)>>* Handlers =
        MessageHandlers.Find(Message.EventId);
    if (!Handlers)
    {
        return;
    }

    for (TFunction<void(const FUIMessage&)>& Handler : *Handlers)
    {
        if (Handler)
        {
            Handler(Message);
        }
    }
}

void UUIMessageSubsystem::SendUIMessage(FName EventId, UObject* Source)
{
    Send(EventId, Source);
}

void UUIMessageSubsystem::RegisterObject(FName ObjectId, UObject* Object)
{
    if (!Object)
    {
        ObjectRegistry.Remove(ObjectId);
        return;
    }

    ObjectRegistry.Add(ObjectId, Object);
}
