#pragma once

#include "CoreMinimal.h"
#include "Vehicles/VehicleMaster.h"
#include "WalkerVehicles.generated.h"

class ACameraVehicle;
class UVehiclesAnimInstance;

UCLASS()
class LANDVEHICULE_API AWalkerVehicles : public AVehicleMaster
{
	GENERATED_BODY()

public:
	AWalkerVehicles();

protected:
	virtual void BeginPlay() override;

private:
	/*- Variables -*/
	UPROPERTY()
	FRotator CurrentAngle;
	UPROPERTY()
	float AccumulatedYaw = 0.0f;
	UPROPERTY()
	float AccumulatedPitch = 0.0f;

	/*- Function -*/
	UFUNCTION(BlueprintCallable)
	void SetTurretRotation(ACameraVehicle* CurrenCamera, FRotator TurretAngle);

	UFUNCTION(BlueprintCallable)
	void ApplyTurretRotation(float DeltaYaw, float DeltaPitch);

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FRotator GetTurretAngle(ACameraVehicle* CurrenCamera, float InterpSpeed = 1.f);
};
