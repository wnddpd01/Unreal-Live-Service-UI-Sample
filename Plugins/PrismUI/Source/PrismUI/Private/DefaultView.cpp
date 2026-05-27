#include "DefaultView.h"

#include "UIMessageSubsystem.h"
#include "Engine/Engine.h"

void UDefaultView::NativeConstruct()
{
    Super::NativeConstruct();

    SendViewEvent(MakeViewEventId(TEXT("Constructed")));
}

void UDefaultView::SendViewEvent(FName EventId)
{
    if (EventId.IsNone() || !GEngine)
    {
        return;
    }

    const FName NormalizedEventId = NormalizeViewEventId(EventId);
    if (NormalizedEventId.IsNone())
    {
        return;
    }

    if (!IsDeclaredViewEvent(NormalizedEventId))
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("UDefaultView '%s' blocked undeclared event '%s'. Add it to DeclaredViewEvents first."),
            *GetName(),
            *NormalizedEventId.ToString()
        );
        return;
    }

    if (UUIMessageSubsystem* Messages =
        GEngine->GetEngineSubsystem<UUIMessageSubsystem>())
    {
        Messages->RegisterObject(DefaultViewPresentationIds::View, this);
        Messages->Send(NormalizedEventId, this);
    }
}

FName UDefaultView::MakeViewEventId(FName EventName) const
{
    if (ViewId.IsNone() || EventName.IsNone())
    {
        return NAME_None;
    }

    return FName(*FString::Printf(
        TEXT("View.%s.%s"),
        *ViewId.ToString(),
        *EventName.ToString()
    ));
}

FName UDefaultView::NormalizeViewEventId(FName EventId) const
{
    if (EventId.IsNone())
    {
        return NAME_None;
    }

    const FString EventIdString = EventId.ToString();
    if (EventIdString.StartsWith(TEXT("View.")))
    {
        return EventId;
    }

    FString Prefix;
    FString Remainder;
    if (EventIdString.Split(TEXT("."), &Prefix, &Remainder) &&
        Prefix == ViewId.ToString())
    {
        return MakeViewEventId(FName(*Remainder));
    }

    return MakeViewEventId(EventId);
}

bool UDefaultView::IsDeclaredViewEvent(FName EventId) const
{
    if (EventId == MakeViewEventId(TEXT("Constructed")))
    {
        return true;
    }

    for (const FName& EventName : DeclaredViewEvents)
    {
        if (EventId == NormalizeViewEventId(EventName))
        {
            return true;
        }
    }

    return false;
}
