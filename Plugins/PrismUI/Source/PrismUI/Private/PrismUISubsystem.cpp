#include "PrismUISubsystem.h"

#include "UIMessagePresenterRegistry.h"

void UPrismUISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    UIMessagePresenterRegistry::InstallAll(*this);

    UE_LOG(LogTemp, Log, TEXT("UIMessageSubsystem initialized."));
}

void UPrismUISubsystem::Deinitialize()
{
    MessageHandlers.Reset();

    UE_LOG(LogTemp, Log, TEXT("UIMessageSubsystem deinitialized."));

    Super::Deinitialize();
}

void UPrismUISubsystem::Send(FName EventId, UObject* Source, UObject* Target)
{
    FUIMessage Message;
    Message.EventId = EventId;
    Message.Source = Source;
    Message.Target = Target;

    Send(Message);
}

void UPrismUISubsystem::Send(const FUIMessage& Message)
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

void UPrismUISubsystem::SendUIMessage(FName EventId, UObject* Source, UObject* Target)
{
    Send(EventId, Source, Target);
}
