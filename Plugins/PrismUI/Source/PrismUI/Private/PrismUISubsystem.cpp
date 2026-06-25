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
    MessageSubscriptions.Reset();
    UIBindStates.Reset();

    UE_LOG(LogTemp, Log, TEXT("UIMessageSubsystem deinitialized."));

    Super::Deinitialize();
}

void UPrismUISubsystem::ClearPresenterSubscriptions(FName PresenterName)
{
    for (auto It = MessageSubscriptions.CreateIterator(); It; ++It)
    {
        It.Value().RemoveAll(
            [PresenterName](const FUIMessageSubscription& Subscription)
            {
                return Subscription.PresenterName == PresenterName;
            }
        );

        if (It.Value().Num() == 0)
        {
            It.RemoveCurrent();
        }
    }
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

    TArray<FUIMessageSubscription>* Subscriptions = MessageSubscriptions.Find(Message.EventId);
    if (!Subscriptions)
    {
        return;
    }

    for (FUIMessageSubscription& Subscription : *Subscriptions)
    {
        if (Subscription.Callback)
        {
            Subscription.Callback(Message);
        }
    }
}

void UPrismUISubsystem::SendUIMessage(FName EventId, UObject* Source, UObject* Target)
{
    Send(EventId, Source, Target);
}

void UPrismUISubsystem::Bind(const FPrismUIBindRequest& bindingRequest)
{
    if (bindingRequest.PresenterName.IsNone())
    {
        UE_LOG(LogTemp, Warning, TEXT("PrismUI ignored bind request without PresenterName."));
        return;
    }

    TSharedPtr< FPrismUIBindState > pBindState = MakeShared< FPrismUIBindState >();
    pBindState->BindingId = bindingRequest.BindingId.IsNone() ? bindingRequest.PresenterName : bindingRequest.BindingId;
    pBindState->PresenterName = bindingRequest.PresenterName;
    pBindState->Model = bindingRequest.Model;
    pBindState->View = bindingRequest.View;

    UIBindStates.Add(pBindState->BindingId, pBindState);
}
