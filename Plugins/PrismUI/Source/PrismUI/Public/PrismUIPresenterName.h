// PrismUIPresenterName.h
#pragma once

#include "CoreMinimal.h"

#define PRISMUI_PRESENTER(PresenterType) \
public: \
    static FName PresenterName() \
    { \
        static const FName Name(TEXT(#PresenterType)); \
        return Name; \
    }