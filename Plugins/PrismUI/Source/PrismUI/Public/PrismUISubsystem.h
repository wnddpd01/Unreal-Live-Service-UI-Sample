#pragma once

#include "CoreMinimal.h"
#include "PrismUITypes.h"
#include "Subsystems/EngineSubsystem.h"
#include "PrismUISubsystem.generated.h"

struct PRISMUI_API FUIMessageSubscription
{
    FName PresenterName;
    TFunction<void(const FUIMessage&)> Callback;
};

UCLASS()
class PRISMUI_API UPrismUISubsystem : public UEngineSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

public:
    template <typename HandlerType>
    void Subscribe(FName PresenterName, FName EventId, HandlerType&& Handler)
    {
        FUIMessageSubscription Subscription;
        Subscription.PresenterName = PresenterName;
        Subscription.Callback = TFunction<void(const FUIMessage&)>( Forward<HandlerType>(Handler) );
        
        MessageSubscriptions.FindOrAdd(EventId).Add(MoveTemp(Subscription));
    }
    
    void ClearPresenterSubscriptions(FName PresenterName);

    void Send(FName EventId, UObject* Source = nullptr);
    void Send(const FUIMessage& Message);

    UFUNCTION(BlueprintCallable, Category = "UI|Messages")
    void SendUIMessage(FName EventId, UObject* Source = nullptr);

    void Bind(const FPrismUIBindRequest& bindingRequest);
    bool Unbind(const FName BindingId);

    const TSharedPtr<FPrismUIBindState> FindBindState(FName BindingId) const
    {
        if (const TSharedPtr<FPrismUIBindState>* FoundState = UIBindStates.Find(BindingId))
        {
            if (FoundState->IsValid() && (*FoundState)->IsValid() )
            {
                return *FoundState;
            }
        }
        return nullptr;
    }

private :
    void PurgeStaleBindings();

private:
    TMap<FName/* Event Id */, TArray<FUIMessageSubscription>> MessageSubscriptions;
    TMap<FName, TSharedPtr< FPrismUIBindState > > UIBindStates;

    FDelegateHandle PostGarbageCollectHandle;
};
