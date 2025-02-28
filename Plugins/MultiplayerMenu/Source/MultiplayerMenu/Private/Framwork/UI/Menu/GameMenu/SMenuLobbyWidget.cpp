#include "Framwork/UI/Menu/GameMenu/SMenuLobbyWidget.h"

void USMenuLobbyWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
}

USPlayerLobbyWidget* USMenuLobbyWidget::GetPlayerLobbyWidget()
{
	if (PlayerLobbyWidget)
		return PlayerLobbyWidget;

	return nullptr;
}
