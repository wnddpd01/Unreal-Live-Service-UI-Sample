// PrismUIPresenterId.h
#pragma once

#include "CoreMinimal.h"

#define PRISMUI_PRESENTER_ID(PresenterType) \
public: \
    static FName PresenterId() \
    { \
        static const FName Id(TEXT(#PresenterType)); \
        return Id; \
    }