#include "Framwork/UI/Menu/GameMenu/SPlayerLobbyEntry.h"
#include "CommonTextBlock.h"
#include "Framwork/SLobbyPlayerController.h"

void USPlayerLobbyEntry::NativeConstruct()
{
	Super::NativeConstruct();

	Controller = Cast<ASLobbyPlayerController>(GetOwningPlayer());

	PlayerNameText->SetText(FText::FromString(Controller->PlayerInfo.PlayerName));
}

void USPlayerLobbyEntry::SetPlayerReady(bool bReady)
{
	Controller->PlayerInfo.bIsReady = bReady;
	if (Controller->PlayerInfo.bIsReady)
	{
		PlayerReadyText->SetText(FText::FromString("Ready"));
	}
	else
	{
		PlayerReadyText->SetText(FText::FromString("Not ready."));
	}
}
