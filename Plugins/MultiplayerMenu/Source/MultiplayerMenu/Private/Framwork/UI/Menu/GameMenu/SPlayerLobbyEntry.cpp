#include "Framwork/UI/Menu/GameMenu/SPlayerLobbyEntry.h"
#include "Components/TextBlock.h"
#include "Framwork/SLobbyPlayerController.h"
#include "Framwork/Data/SPlayerData.h"


void USPlayerLobbyEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	PlayerData = Cast<USPlayerData>(ListItemObject);
	UpdateEntry();
}

void USPlayerLobbyEntry::UpdateEntry()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Blue, PlayerData->PlayerInfo.PlayerName);
	
	PlayerNameText->SetText(FText::FromString(PlayerData->PlayerInfo.PlayerName));
	SetPlayerReady();
}

void USPlayerLobbyEntry::SetPlayerReady()
{
	bool bIsReady = PlayerData->PlayerInfo.bIsReady;
	PlayerReadyText->SetText(FText::FromString(bIsReady ? "Ready" : "Not ready"));
}

