#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "PrismUISubsystem.generated.h"

USTRUCT(BlueprintType)
struct PRISMUI_API FUIMessage
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "UI Message")
    FName EventId;

    TWeakObjectPtr<UObject> Source;

    UObject* GetSource() const
    {
        return Source.Get();
    }

    TWeakObjectPtr<UObject> Target;

    UObject* GetTarget() const
    {
        return Target.Get();
    }
};

struct PRISMUI_API FUIMessageSubscription
{
    FName PresenterName;
    TFunction<void(const FUIMessage&)> Callback;
};

USTRUCT(BlueprintType)
struct FPrismUIBindRequest
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName BindingId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName PresenterName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UObject> Model = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UObject> View = nullptr;
};

struct FPrismUIBindState
{
    FName BindingId;
    FName PresenterName;
    TWeakObjectPtr<UObject> Model;
    TWeakObjectPtr<UObject> View;

	bool IsValid() const
	{
		return Model.IsValid() && View.IsValid();
	}
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

    void Send(FName EventId, UObject* Source = nullptr, UObject* Target = nullptr);
    void Send(const FUIMessage& Message);

    UFUNCTION(BlueprintCallable, Category = "UI|Messages")
    void SendUIMessage(FName EventId, UObject* Source = nullptr, UObject* Target = nullptr);

    void Bind(const FPrismUIBindRequest& bindingRequest);
    bool Unbind(const FName BindingId);

    const TSharedPtr<FPrismUIBindState> GetBindState(FName BindingId) const
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
