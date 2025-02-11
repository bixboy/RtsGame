#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameMode.generated.h"

class ASPlayerController;
class USGameData;

UCLASS()
class MULTIPLAYERMENU_API ASGameMode : public AGameModeBase
{
	GENERATED_BODY()

	ASGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void InitGameState() override;
	virtual void GenericPlayerInitialization(AController* C) override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	virtual bool PlayerCanRestart_Implementation(APlayerController* Player) override;
	virtual void FailedToRestartPlayer(AController* NewPlayer) override;

	UFUNCTION()
	void RequestPlayerRestartNextFrame(AController* Controller, bool bForceReset = false);

protected:
	UFUNCTION()
	void HandleGameDataAssignment();
	UFUNCTION()
	void OnGameDataAssignment();
	UFUNCTION()
	void OnGameDataLoaded();
	UFUNCTION()
	bool IsGameDataLoaded() const { return CurrentGameDataAssetId.IsValid(); }
	
	UFUNCTION()
	USGameData* GetGameData() const;
	
	UFUNCTION()
	void OnGameReady();

	UFUNCTION()
	void InitialisePlayer();
	
	UPROPERTY()
	bool bGameReady = false;

	UPROPERTY()
	FPrimaryAssetId CurrentGameDataAssetId;

	UPROPERTY()
	TArray<ASPlayerController*> SessionPlayers;
};
