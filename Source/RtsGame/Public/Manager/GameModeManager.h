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
class RTSGAME_API AGameModeManager : public AGameMode
{
	GENERATED_BODY()
	
public:
	
	virtual void StartPlay() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	UFUNCTION()
	float GetElapsedTime() const;

protected:

	UFUNCTION()
	void BeginMatch();

	UFUNCTION()
	void CheckForVictory();

	UFUNCTION()
	void EndMatch();
	
	UPROPERTY()
	FGameSettings GameSettings;

	UPROPERTY()
	EMatchState MatchState;

	UPROPERTY()
	float GameStartTime;

	UPROPERTY()
	TMap<int, APlayerController*> PlayersTeam;

	UPROPERTY()
	int32 NextTeamId = 0;

};
