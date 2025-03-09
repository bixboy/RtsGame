#pragma once

#include "CoreMinimal.h"
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

protected:
	UFUNCTION()
	void NetworkFailureHappened(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type Arg, const FString& String);

	UFUNCTION()
	void TravelFailureHappened(UWorld* World, ETravelFailure::Type Arg, const FString& String);

	UPROPERTY()
	FTimerHandle CheckSessionTimerHandle;
};
