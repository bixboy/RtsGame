#include "Framwork/UI/Menu/Multiplayer/SGameSessionButton.h"
#include "CommonTextBlock.h"


void USGameSessionButton::SetTextDisplays(const FText& Game, const FText& Map, const FText& Ping,
	const FText& PlayerCount, const FText& MaxPlayerCount)
{
	if (ButtonTextBlock)
		SetButtonText(Game);

	if (MapText)
		MapText->SetText(Map);

	if (PingText)
		PingText->SetText(Ping);

	if (PlayerCountText)
		PlayerCountText->SetText(PlayerCount);

	if (MaxPlayerCountText)
		MaxPlayerCountText->SetText(MaxPlayerCount);
}
