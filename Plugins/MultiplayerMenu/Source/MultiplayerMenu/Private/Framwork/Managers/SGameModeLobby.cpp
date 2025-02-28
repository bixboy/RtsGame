#include "Framwork/Managers/SGameModeLobby.h"
#include "AdvancedSessionsLibrary.h"
#include "Framwork/Managers/SLobbyGameState.h"

ASGameModeLobby::ASGameModeLobby(const FObjectInitializer& ObjectInitializer)
{
	bUseSeamlessTravel = true;
}

void ASGameModeLobby::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	APlayerController* PlayerController = Cast<APlayerController>(NewPlayer);
	if (!PlayerController)
		return;
	
	Controllers.AddUnique(PlayerController);

	
	// Update Widget
	APlayerState* NewPlayerState = NewPlayer->PlayerState;
	if (!NewPlayerState)
		return;

	ASLobbyGameState* LobbyGameState = GetGameState<ASLobbyGameState>();
	if (LobbyGameState)
	{
		LobbyGameState->ConnectedPlayers.Add(NewPlayerState);
		LobbyGameState->Multicast_OnPlayerJoined(NewPlayerState);
	}
	
	/* TSharedPtr<FOnlineSessionSettings> SessionSettings = UAdvancedSessionsLibrary::GetSessionSettings();
	if (SessionSettings.IsValid())
	{

	} */
}

void ASGameModeLobby::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if (APlayerController* PlayerController = Cast<APlayerController>(Exiting))
	{
		Controllers.Remove(PlayerController);
	}
}
