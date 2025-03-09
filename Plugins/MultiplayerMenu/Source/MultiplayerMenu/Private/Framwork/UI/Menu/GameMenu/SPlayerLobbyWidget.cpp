#include "Framwork/UI/Menu/GameMenu/SPlayerLobbyWidget.h"
#include "Components/ListView.h"
#include "Framwork/Data/SPlayerData.h"


void USPlayerLobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void USPlayerLobbyWidget::UpdatePlayerList(TArray<FPlayerInfo> PlayersList)
{
	PlayerLobbyView->ClearListItems();
	
	for (FPlayerInfo Player : PlayersList)
	{
		if (USPlayerData* NewPlayerData = NewObject<USPlayerData>(this))
		{
			NewPlayerData->PlayerInfo = Player;
			
			PlayerLobbyView->AddItem(NewPlayerData);
			PlayerLobbyEntries.Add(NewPlayerData);
		}
	}
}
