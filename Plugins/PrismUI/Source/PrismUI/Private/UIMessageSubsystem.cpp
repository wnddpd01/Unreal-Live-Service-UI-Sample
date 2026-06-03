#include "UIMessageSubsystem.h"

#include "UIMessagePresenterRegistry.h"

void UUIMessageSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    UIMessagePresenterRegistry::InstallAll(*this);

    UE_LOG(LogTemp, Log, TEXT("UIMessageSubsystem initialized."));
}

void UUIMessageSubsystem::Deinitialize()
{
    MessageHandlers.Reset();

    UE_LOG(LogTemp, Log, TEXT("UIMessageSubsystem deinitialized."));

    Super::Deinitialize();
}

void UUIMessageSubsystem::Send(FName EventId, UObject* Source, UObject* Target)
{
    FUIMessage Message;
    Message.EventId = EventId;
    Message.Source = Source;
    Message.Target = Target;

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

void UUIMessageSubsystem::SendUIMessage(FName EventId, UObject* Source, UObject* Target)
{
    Send(EventId, Source, Target);
}
