#include "DefaultView.h"

#include "UIPresentationSubsystem.h"
#include "Engine/Engine.h"

void UDefaultView::NativeConstruct()
{
    Super::NativeConstruct();

    EmitViewEvent(DefaultViewPresentationIds::Constructed);
}

void UDefaultView::EmitViewEvent(FName EventId)
{
    if (EventId.IsNone() || !GEngine)
    {
        return;
    }

    if (UUIPresentationSubsystem* Presentation =
        GEngine->GetEngineSubsystem<UUIPresentationSubsystem>())
    {
        Presentation->RegisterObject(DefaultViewPresentationIds::View, this);
        Presentation->Emit(EventId);
    }
}
