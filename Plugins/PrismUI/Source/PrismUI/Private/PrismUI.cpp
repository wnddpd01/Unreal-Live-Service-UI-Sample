#include "PrismUI.h"
#include "Modules/ModuleManager.h"
#include "PrismUISubsystem.h"

IMPLEMENT_MODULE(FDefaultModuleImpl, PrismUI)

namespace
{
	UPrismUISubsystem* GetPrismUISubsystem()
	{
		if (!GEngine)
		{
			UE_LOG(LogTemp, Warning, TEXT("PrismUISubsystem not found."));
			return nullptr;
		}
		return GEngine->GetEngineSubsystem<UPrismUISubsystem>();
	}
}

namespace UE
{
	namespace PrismUI
	{

		PRISMUI_API void SendMessage(FName EventId, UObject* Source)
		{
			UPrismUISubsystem* PrismUISubsystem = GetPrismUISubsystem();
			if (!PrismUISubsystem) 
			{
				return;
			}

			PrismUISubsystem->Send(EventId, Source);
		}

		PRISMUI_API void Bind(const FPrismUIBindRequest& Request)
		{
			UPrismUISubsystem* PrismUISubsystem = GetPrismUISubsystem();
			if (!PrismUISubsystem) 
			{
				return;
			}

			PrismUISubsystem->Bind(Request);
		}

		PRISMUI_API void Unbind(FName BindingId)
		{
			UPrismUISubsystem* PrismUISubsystem = GetPrismUISubsystem();
			if (!PrismUISubsystem) 
			{
				return;
			}

			PrismUISubsystem->Unbind(BindingId);
		}

		PRISMUI_API TSharedPtr<const FPrismUIBindState> FindBindState(FName BindingId)
		{
			UPrismUISubsystem* PrismUISubsystem = GetPrismUISubsystem();
			if (!PrismUISubsystem) 
			{
				return nullptr;
			}

			return PrismUISubsystem->FindBindState(BindingId);
		}
	}
}