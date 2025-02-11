#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SGameState.generated.h"

class UCommonActivatableWidget;
class USGameData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameStateReadyDelegate);

UCLASS()
class MULTIPLAYERMENU_API ASGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ASGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void SetGameData(USGameData* NewGameData);

	UFUNCTION()
	bool ShouldShowMenu() const;

	UFUNCTION()
	TSoftClassPtr<UCommonActivatableWidget> GetMenuClass() const;

	FOnGameStateReadyDelegate OnGameStateReady;

protected:
	UFUNCTION()
	void OnRep_GameData();

	UPROPERTY(Replicated = OnRep_GameData)
	USGameData* GameData;
};
