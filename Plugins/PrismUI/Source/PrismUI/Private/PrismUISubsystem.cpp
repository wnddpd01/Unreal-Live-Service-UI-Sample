#include "PrismUISubsystem.h"

#include "UIMessagePresenterRegistry.h"
#include "UObject/UObjectGlobals.h"

void UPrismUISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    UIMessagePresenterRegistry::InstallAll(*this);
    PostGarbageCollectHandle = FCoreUObjectDelegates::GetPostGarbageCollect().AddUObject(
        this,
        &UPrismUISubsystem::PurgeStaleBindings
    );

    UE_LOG(LogTemp, Log, TEXT("UIMessageSubsystem initialized."));
}

void UPrismUISubsystem::Deinitialize()
{
    FCoreUObjectDelegates::GetPostGarbageCollect().Remove(PostGarbageCollectHandle);
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


void UPrismUISubsystem::Send(FName EventId, UObject* Source)
{
    FUIMessage Message;
    Message.EventId = EventId;
    Message.Source = Source;

    Send(Message);
}

void UPrismUISubsystem::Send(const FUIMessage& Message)
{
    if (Message.EventId.IsNone())
    {
        return;
    }

    const TArray<FUIMessageSubscription>* Subscriptions = MessageSubscriptions.Find(Message.EventId);
    if (!Subscriptions)
    {
        return;
    }

    // Dispatch uses a snapshot captured at Send() start
	// Subscription changes made by callbacks apply from the next Send() call.
    const TArray<FUIMessageSubscription> SubscriptionsCopy = *Subscriptions;

    for (const FUIMessageSubscription& Subscription : SubscriptionsCopy)
    {
        if (Subscription.Callback)
        {
            Subscription.Callback(Message);
        }
    }
}

void UPrismUISubsystem::SendUIMessage(FName EventId, UObject* Source)
{
    Send(EventId, Source);
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

bool UPrismUISubsystem::Unbind(const FName BindingId)
{
	if (BindingId.IsNone())
	{
		return false;
	}

    return UIBindStates.Remove(BindingId) > 0;
}

void UPrismUISubsystem::PurgeStaleBindings()
{
	for (auto It = UIBindStates.CreateIterator(); It; ++It)
	{
		const TSharedPtr<FPrismUIBindState>& BindState = It.Value();
        if (!BindState.IsValid() || !BindState->Model.IsValid() || !BindState->View.IsValid() )
        {
            It.RemoveCurrent();
        }
	}
}
