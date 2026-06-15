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

void UPrismUISubsystem::Bind(const FPrismUIBindRequest& bindingRequest)
{
    if (bindingRequest.BindingId.IsNone())
    {
        UE_LOG(LogTemp, Warning, TEXT("PrismUI ignored bind request without BindingId."));
        return;
    }

    TSharedPtr< FPrismUIBindState > pBindState = MakeShared< FPrismUIBindState >();
    pBindState->BindingId = bindingRequest.BindingId;
    pBindState->Model = bindingRequest.Model;
    pBindState->View = bindingRequest.View;

    UIBindStates.Add(pBindState->BindingId, pBindState);
}
