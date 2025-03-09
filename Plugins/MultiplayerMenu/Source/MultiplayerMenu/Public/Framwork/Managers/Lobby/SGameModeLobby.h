#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "SGameModeLobby.generated.h"

struct FPlayerInfo;
class ASLobbyPlayerController;


UCLASS()
class MULTIPLAYERMENU_API ASGameModeLobby : public AGameMode
{
	GENERATED_BODY()

public:

	ASGameModeLobby(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void OnPostLogin(AController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;

	UFUNCTION()
	void UpdatePlayerInfo(FPlayerInfo NewPlayerInfo, APlayerController* Controller);

	UFUNCTION()
	FPlayerInfo GetPlayerInfo(APlayerState* PlayerState);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetAllPlayersReady();

protected:

	UPROPERTY()
	TArray<FPlayerInfo> PlayersInfo;

	UFUNCTION()
	FPlayerInfo InitPlayerInfo(ASLobbyPlayerController* PlayerController);

	UFUNCTION()
	void DelayedUpdatePlayersList();
	
	UPROPERTY()
	TArray<ASLobbyPlayerController*> Controllers;

	UPROPERTY()
	ASLobbyPlayerController* NewController;
};
