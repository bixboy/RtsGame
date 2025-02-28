#include "Framwork/SLobbyPlayerController.h"
#include "AdvancedSessionsLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Framwork/Managers/SLobbyGameState.h"
#include "Framwork/UI/Menu/GameMenu/SMenuLobbyWidget.h"
#include "Framwork/UI/Menu/GameMenu/SPlayerLobbyWidget.h"
#include "GameFramework/PlayerState.h"

ASLobbyPlayerController::ASLobbyPlayerController(const FObjectInitializer& ObjectInitializer)
{
}

void ASLobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	UAdvancedSessionsLibrary::GetPlayerName(this, PlayerInfo.PlayerName);
	
	if (ASLobbyGameState* LobbyGameState = GetWorld()->GetGameState<ASLobbyGameState>())
		LobbyGameState->OnPlayersChanged.AddDynamic(this, &ASLobbyPlayerController::RefreshPlayerList);

	if (!LobbyWidget)
		SetupWidget();


	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ASLobbyPlayerController::RefreshPlayerList, 1.0f, false);
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

void ASLobbyPlayerController::RefreshPlayerList()
{
	ASLobbyGameState* LobbyGameState = GetWorld()->GetGameState<ASLobbyGameState>();
	if (!LobbyGameState)
		return;
    
	UE_LOG(LogTemp, Log, TEXT("RefreshPlayerList: Updating UI with %d players."), LobbyGameState->ConnectedPlayers.Num());
	for (APlayerState* TempPlayerState : LobbyGameState->ConnectedPlayers)
	{
		if (TempPlayerState)
		{
			if (APlayerController* PC = Cast<APlayerController>(TempPlayerState->GetOwningController()))
			{
				OnPlayerJoinSession(PC);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("RefreshPlayerList: PlayerState %s has null Owner."), *TempPlayerState->GetPlayerName());
			}
		}
	}
}

void ASLobbyPlayerController::OnPlayerJoinSession(APlayerController* NewController)
{
	if (!LobbyWidget)
		SetupWidget();

	if (LobbyWidget)
		LobbyWidget->GetPlayerLobbyWidget()->AddPlayerToWidget(NewController);
}



