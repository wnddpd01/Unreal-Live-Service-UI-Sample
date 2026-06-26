#pragma once

#include "CoreMinimal.h"
#include "PrismUITypes.h"

namespace UE
{
    namespace PrismUI
    {
        PRISMUI_API void SendMessage(FName EventId, UObject* Source = nullptr);

        PRISMUI_API void Bind(const FPrismUIBindRequest& Request);
        PRISMUI_API void Unbind(FName BindingId);

        PRISMUI_API TSharedPtr<const FPrismUIBindState> FindBindState(FName BindingId);
    }
}
