#pragma once

#include "CoreMinimal.h"
#include "CreateSessionCallbackProxyAdvanced.h"
#include "Framwork/UI/Menu/Multiplayer/SMenuhostSessionWidget.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SGameInstanceSubSystem.generated.h"



UCLASS()
class MULTIPLAYERMENU_API USGameInstanceSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION()
	void LeaveSession();

	UFUNCTION(BlueprintImplementableEvent)
	void DestroySession(UWorld* WorldContext, APlayerController* PlayerController);

	UFUNCTION()
	void CheckSessionEmpty();

	UFUNCTION()
	FGameSettings GetGameSettings();

protected:
	UFUNCTION()
	void NetworkFailureHappened(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type Arg, const FString& String);

	UFUNCTION()
	void TravelFailureHappened(UWorld* World, ETravelFailure::Type Arg, const FString& String);

	UFUNCTION(BlueprintCallable)
	void OnDestroySessionComplete();
	
	UPROPERTY()
	FTimerHandle CheckSessionTimerHandle;

// ------- Hosting -------- //
public:

	UFUNCTION()
	void HostSession(const FGameSettings Settings, TSoftObjectPtr<UWorld> LevelToOpen);

	UFUNCTION()
	FString GetLevelPath(FSoftObjectPath Level);

protected:

	UFUNCTION()
	void HandleHostSuccess();

	UFUNCTION()
	void HandleHostFailure();

	UPROPERTY()
	UCreateSessionCallbackProxyAdvanced* CreateProxy = nullptr;

	UPROPERTY()
	FGameSettings PendingSettings;

	UPROPERTY()
	TSoftObjectPtr<UWorld> LevelLobby;
};
