#pragma once
#include "CoreMinimal.h"
#include "Framwork/UI/Menu/Multiplayer/SMenuhostSessionWidget.h"
#include "GameFramework/GameMode.h"
#include "GameModeManager.generated.h"

UENUM(BlueprintType)
enum class EMatchState : uint8
{
	WaitingToStart UMETA(DisplayName = "WaitingToStart"),
	InProgress    UMETA(DisplayName = "InProgress"),
	Completed     UMETA(DisplayName = "Completed")
};

UCLASS()
class RTSGAME_API AGameModeManager : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	
	virtual void StartPlay() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	UFUNCTION()
	float GetElapsedTime() const;

	UFUNCTION()
	int GetPlayerTeam(APlayerController* Player);

	UFUNCTION()
	APlayerController* GetPlayerByTeam(int TeamNum);

protected:

	UFUNCTION()
	void BeginMatch();

	UFUNCTION()
	void CheckForVictory();

	UFUNCTION()
	void EndGameMatch();

	UFUNCTION()
	void AssignTeamToPlayer(APlayerController* PC);
	
	UPROPERTY()
	FGameSettings GameSettings;

	UPROPERTY()
	EMatchState GameMatchState;

	UPROPERTY()
	float GameStartTime;

	UPROPERTY()
	TMap<int, APlayerController*> PlayersTeam;

	UPROPERTY()
	int32 NextTeamId = 1;

};
