#include "Framwork/Managers/SGameState.h"
#include "CommonActivatableWidget.h"
#include "Framwork/Data/SGameData.h"
#include "Net/UnrealNetwork.h"

ASGameState::ASGameState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

void ASGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, GameData);
}


void ASGameState::SetGameData(USGameData* NewGameData)
{
	if (!HasAuthority()) return;
	
	if (NewGameData)
	{
		GameData = NewGameData;
		OnRep_GameData();
	}
}

bool ASGameState::ShouldShowMenu() const
{
	if (GameData)
		return GameData->bShowMenu;

	return false;
}

TSoftClassPtr<UCommonActivatableWidget> ASGameState::GetMenuClass() const
{
	if (GameData)
		return GameData->MainMenuClass;
	
	return nullptr;
}

void ASGameState::OnRep_GameData()
{
	if (HasAuthority())
	{
		OnGameStateReady.Broadcast();
	}
}
