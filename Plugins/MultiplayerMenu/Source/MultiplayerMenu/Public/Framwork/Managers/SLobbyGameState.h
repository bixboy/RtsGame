#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SLobbyGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayersChanged);

UCLASS()
class MULTIPLAYERMENU_API ASLobbyGameState : public AGameState
{
	GENERATED_BODY()

public:
	ASLobbyGameState(const FObjectInitializer& ObjectInitializer);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_Players();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnPlayerJoined(APlayerState* NewPlayerState);

	UPROPERTY(ReplicatedUsing = OnRep_Players)
	TArray<APlayerState*> ConnectedPlayers;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPlayersChanged OnPlayersChanged;
};
