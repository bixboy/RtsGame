#include "Framwork/SLobbyPlayerController.h"

#include "EnhancedInputComponent.h"
#include "Blueprint/UserWidget.h"
#include "Framwork/Components/SChatComponent.h"
#include "Framwork/Managers/Lobby/SGameModeLobby.h"
#include "Framwork/Managers/Lobby/SLobbyPlayerState.h"
#include "Framwork/UI/Menu/GameMenu/SMenuLobbyWidget.h"
#include "Framwork/UI/Menu/GameMenu/SPlayerLobbyWidget.h"


ASLobbyPlayerController::ASLobbyPlayerController(const FObjectInitializer& ObjectInitializer)
{
	ChatComponent = CreateDefaultSubobject<USChatComponent>(TEXT("ChatComponent"));
}

void ASLobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!LobbyWidget)
		SetupWidget();
}

void ASLobbyPlayerController::SetupWidget()
{
	if (!IsLocalController())
		return;
	
	LobbyWidget = CreateWidget<USMenuLobbyWidget>(this, LobbyWidgetClass);
	
	if (LobbyWidget)
		LobbyWidget->AddToViewport(9999);
	else
		UE_LOG(LogTemp, Error, TEXT("Echec de la création de LobbyWidget"));
	
	FInputModeGameAndUI Mode;
	Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	Mode.SetHideCursorDuringCapture(false);
	
	SetInputMode(Mode);
	SetShowMouseCursor(true);
}

void ASLobbyPlayerController::Client_UpdateWidget_Implementation(const TArray<FPlayerInfo>& PlayersInfo)
{
	if (!LobbyWidget) return;

	LobbyWidget->GetPlayerLobbyWidget()->UpdatePlayerList(PlayersInfo);
	
	if (!PlayerInfo.PlayerState)
	{
		Server_SetPlayerInfo();
	}
}


void ASLobbyPlayerController::Server_SetPlayerReady_Implementation()
{
	FPlayerInfo NewInfo = PlayerInfo;
	NewInfo.bIsReady = !NewInfo.bIsReady;
	
	PlayerInfo.bIsReady = NewInfo.bIsReady;
	GetWorld()->GetAuthGameMode<ASGameModeLobby>()->UpdatePlayerInfo(PlayerInfo, this);
}

FPlayerInfo ASLobbyPlayerController::GetPlayerInfo()
{
	return PlayerInfo;
}

void ASLobbyPlayerController::Server_SetPlayerInfo_Implementation()
{
	PlayerInfo = GetWorld()->GetAuthGameMode<ASGameModeLobby>()->GetPlayerInfo(PlayerState);
}

USChatComponent* ASLobbyPlayerController::GetChatComponent()
{
	return ChatComponent;
}
