#include "Framwork/Managers/Lobby/SGameModeLobby.h"
#include "AdvancedSessionsLibrary.h"
#include "Framwork/SLobbyPlayerController.h"
#include "Framwork/Managers/Lobby/SLobbyGameState.h"
#include "Framwork/Managers/Lobby/SLobbyPlayerState.h"


ASGameModeLobby::ASGameModeLobby(const FObjectInitializer& ObjectInitializer)
{
	bUseSeamlessTravel = true;
}

void ASGameModeLobby::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	ASLobbyPlayerController* PlayerController = Cast<ASLobbyPlayerController>(NewPlayer);
	if (!PlayerController)
		return;
	
	Controllers.AddUnique(PlayerController);
	PlayersInfo.Add(InitPlayerInfo(PlayerController));
	NewController = PlayerController;

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ASGameModeLobby::DelayedUpdatePlayersList, 0.3f, false);
}

void ASGameModeLobby::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if (ASLobbyPlayerController* PlayerController = Cast<ASLobbyPlayerController>(Exiting))
	{
		Controllers.Remove(PlayerController);
		PlayersInfo.Remove(PlayerController->GetPlayerInfo());
		NewController = PlayerController;
		
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ASGameModeLobby::DelayedUpdatePlayersList, 0.3f, false);
	}
}

FPlayerInfo ASGameModeLobby::InitPlayerInfo(ASLobbyPlayerController* PlayerController)
{
	FString NewPlayerName;
	UAdvancedSessionsLibrary::GetPlayerName(PlayerController, NewPlayerName);
	if (NewPlayerName.IsEmpty())
		NewPlayerName = TEXT("Inconnu");

	FPlayerInfo NewInfo;
	NewInfo.PlayerName = NewPlayerName;
	NewInfo.PlayerState = Cast<ASLobbyPlayerState>(PlayerController->PlayerState);
	NewInfo.bIsReady = false;

	return NewInfo;
}

void ASGameModeLobby::UpdatePlayerInfo(FPlayerInfo NewPlayerInfo, APlayerController* Controller)
{
	int Index = GetGameState<ASLobbyGameState>()->PlayerArray.IndexOfByKey(Controller->PlayerState);
	if (Index < 0)
		return;
	
	PlayersInfo[Index] = NewPlayerInfo;
	DelayedUpdatePlayersList();
}

FPlayerInfo ASGameModeLobby::GetPlayerInfo(APlayerState* PlayerState)
{
	int Index = GetGameState<ASLobbyGameState>()->PlayerArray.IndexOfByKey(PlayerState);
	if (Index < 0)
		return FPlayerInfo();
	
	return PlayersInfo[Index];
}

void ASGameModeLobby::DelayedUpdatePlayersList()
{
	for (ASLobbyPlayerController* PlayerController : Controllers)
	{
		PlayerController->Client_UpdateWidget(PlayersInfo);
	}
}

bool ASGameModeLobby::GetAllPlayersReady()
{
	for (const FPlayerInfo& PlayerInfo : PlayersInfo)
	{
		if (!PlayerInfo.bIsReady)
			return false;
	}
	
	return true;
}
