#include "Framwork/Managers/SLobbyGameState.h"
#include "Framwork/SLobbyPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"


ASLobbyGameState::ASLobbyGameState(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
}

void ASLobbyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ASLobbyGameState, ConnectedPlayers);
}

void ASLobbyGameState::OnRep_Players()
{
    UE_LOG(LogTemp, Log, TEXT("OnRep_Players appelé. Nombre de joueurs : %d"), ConnectedPlayers.Num());

    if (OnPlayersChanged.IsBound())
    {
        OnPlayersChanged.Broadcast();
    }
}

void ASLobbyGameState::Multicast_OnPlayerJoined_Implementation(APlayerState* NewPlayerState)
{
    if (!NewPlayerState)
        return;

    UE_LOG(LogTemp, Log, TEXT("Multicast_OnPlayerJoined: Nouveau joueur %s"), *NewPlayerState->GetPlayerName());

    UWorld* World = GetWorld();
    if (!World)
        return;


    for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
    {
        ASLobbyPlayerController* LobbyController = Cast<ASLobbyPlayerController>(It->Get());
        if (LobbyController)
        {
            APlayerController* NewPlayerController = Cast<APlayerController>(NewPlayerState->GetOwner());
            if (NewPlayerController)
            {
                UE_LOG(LogTemp, Log, TEXT("Appel de OnPlayerJoinSession sur %s pour ajouter %s"), *LobbyController->GetName(), *NewPlayerState->GetPlayerName());

                //LobbyController->OnPlayerJoinSession(NewPlayerController);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Multicast_OnPlayerJoined: NewPlayerState->GetOwner() returned null."));
            }
        }
    }
}

