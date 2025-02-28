#include "Framwork/UI/Menu/GameMenu/SPlayerLobbyWidget.h"

#include "AdvancedSessionsLibrary.h"
#include "Components/ListView.h"
#include "Framwork/UI/Menu/GameMenu/SPlayerLobbyEntry.h"


void USPlayerLobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void USPlayerLobbyWidget::AddPlayerToWidget(APlayerController* NewController)
{
	// Créez une nouvelle instance de l'entrée de lobby.
	if (USPlayerLobbyEntry* NewEntry = CreateWidget<USPlayerLobbyEntry>(this, PlayerLobbyEntryClass))
	{
		PlayerLobbyEntries.AddUnique(NewEntry);
		PlayerLobbyView->AddItem(NewEntry);
	}
}
