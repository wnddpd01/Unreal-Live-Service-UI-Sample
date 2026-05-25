#include "UIModelEvent.h"

#include "Engine/Engine.h"
#include "UIPresentationSubsystem.h"

UUIModelEvent* UUIModelEvent::Create(UObject* InOwner)
{
    if (!InOwner)
    {
        return nullptr;
    }

    UUIModelEvent* ModelEvent = NewObject<UUIModelEvent>(InOwner);
    ModelEvent->Initialize(InOwner);
    return ModelEvent;
}

void UUIModelEvent::Initialize(UObject* InOwner)
{
    Owner = InOwner ? InOwner : GetOuter();
    RegisterWithPresentation();
}

void UUIModelEvent::Raise()
{
    if (!bRegisteredWithPresentation)
    {
        RegisterWithPresentation();
    }

    UObject* Source = GetOwner();
    OnRaisedNative.Broadcast(this, Source);
    OnRaised.Broadcast(Source);
}

UObject* UUIModelEvent::GetOwner() const
{
    if (UObject* OwnerObject = Owner.Get())
    {
        return OwnerObject;
    }

    return GetOuter();
}

void UUIModelEvent::BeginDestroy()
{
    UnregisterFromPresentation();

    Super::BeginDestroy();
}

void UUIModelEvent::RegisterWithPresentation()
{
    if (bRegisteredWithPresentation || !GEngine)
    {
        return;
    }

    if (UUIPresentationSubsystem* Presentation =
        GEngine->GetEngineSubsystem<UUIPresentationSubsystem>())
    {
        Presentation->RegisterModelEvent(this);
        bRegisteredWithPresentation = true;
    }
}

void UUIModelEvent::UnregisterFromPresentation()
{
    if (!bRegisteredWithPresentation || !GEngine)
    {
        bRegisteredWithPresentation = false;
        return;
    }

    if (UUIPresentationSubsystem* Presentation =
        GEngine->GetEngineSubsystem<UUIPresentationSubsystem>())
    {
        Presentation->UnregisterModelEvent(this);
    }

    bRegisteredWithPresentation = false;
}
