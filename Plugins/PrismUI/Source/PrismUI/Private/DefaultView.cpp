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

    if (!IsViewEventId(EventId))
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("UDefaultView '%s' blocked invalid view event '%s'. Use a full event id starting with '%s.'."),
            *GetName(),
            *EventId.ToString(),
            *ViewId.ToString()
        );
        return;
    }

    if (!IsDeclaredViewEvent(EventId))
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("UDefaultView '%s' blocked undeclared event '%s'. Add it to DeclaredViewEvents first."),
            *GetName(),
            *EventId.ToString()
        );
        return;
    }

    if (UUIMessageSubsystem* Messages =
        GEngine->GetEngineSubsystem<UUIMessageSubsystem>())
    {
        Messages->Send(EventId, this);
    }
}

FName UDefaultView::MakeViewEventId(FName EventName) const
{
    if (ViewId.IsNone() || EventName.IsNone())
    {
        return NAME_None;
    }

    return FName(*FString::Printf(
        TEXT("%s.%s"),
        *ViewId.ToString(),
        *EventName.ToString()
    ));
}

bool UDefaultView::IsViewEventId(FName EventId) const
{
    if (ViewId.IsNone() || EventId.IsNone())
    {
        return false;
    }

    const FString EventIdString = EventId.ToString();
    const FString ViewPrefix = FString::Printf(TEXT("%s."), *ViewId.ToString());
    return EventIdString.StartsWith(ViewPrefix);
}

bool UDefaultView::IsDeclaredViewEvent(FName EventId) const
{
    if (EventId == MakeViewEventId(TEXT("Constructed")))
    {
        return true;
    }

    for (const FName& DeclaredEventId : DeclaredViewEvents)
    {
        if (EventId == DeclaredEventId)
        {
            return true;
        }
    }

    return false;
}
