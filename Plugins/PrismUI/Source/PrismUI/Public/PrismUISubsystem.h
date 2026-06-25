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
    void Subscribe(FName EventId, HandlerType&& Handler)
    {
        TFunction<void(const FUIMessage&)> WrappedHandler(Forward<HandlerType>(Handler));
        MessageHandlers.FindOrAdd(EventId).Add(MoveTemp(WrappedHandler));
    }

    void Send(FName EventId, UObject* Source = nullptr, UObject* Target = nullptr);
    void Send(const FUIMessage& Message);

    UFUNCTION(BlueprintCallable, Category = "UI|Messages")
    void SendUIMessage(FName EventId, UObject* Source = nullptr, UObject* Target = nullptr);

    void Bind(const FPrismUIBindRequest& bindingRequest);

	const TSharedPtr<FPrismUIBindState> GetBindState(FName BindingId) const
	{
		if (const TSharedPtr<FPrismUIBindState>* FoundState = UIBindStates.Find(BindingId))
		{
			return *FoundState;
		}
		return nullptr;
	}

private:
    TMap<FName, TArray<TFunction<void(const FUIMessage&)>>> MessageHandlers;
	TMap<FName, TSharedPtr< FPrismUIBindState > > UIBindStates;
};
