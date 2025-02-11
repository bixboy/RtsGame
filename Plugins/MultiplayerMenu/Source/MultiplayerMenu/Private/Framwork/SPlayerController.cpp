#include "Framwork/SPlayerController.h"

#include "PrimaryGameLayout.h"
#include "Framwork/Data/StaticGameData.h"
#include "Framwork/Managers/SGameState.h"

ASPlayerController::ASPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

void ASPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (IsLocalController())
	{
		if (const UWorld* WorldContext = GetWorld())
		{
			if (const ASGameState* SGameState = SGameState = Cast<ASGameState>(WorldContext->GetGameState()))
			{
				ShowMenu(SGameState->GetMenuClass());
			}
		}	
	}
	
}

void ASPlayerController::ShowMenu(const TSoftClassPtr<UCommonActivatableWidget>& MenuClass)
{
	if (MenuClass == nullptr)
		return;

	if (const UWorld* WorldContext = GetWorld())
	{
		if (UPrimaryGameLayout* RootLayout = UPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(WorldContext))
		{
			RootLayout->PushWidgetToLayerStackAsync<UCommonActivatableWidget>(UILayerTags::TAG_UI_LAYER_MENU, true, MenuClass,
				[this](EAsyncWidgetLayerState State, UCommonActivatableWidget* Screen)
				{
					switch (State)
					{
						case EAsyncWidgetLayerState::AfterPush:
							return;
						
						case EAsyncWidgetLayerState::Canceled:
							return;
					}
				});
		}
	}
}
