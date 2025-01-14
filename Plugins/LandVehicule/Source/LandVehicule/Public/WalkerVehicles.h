#pragma once

#include "CoreMinimal.h"
#include "VehicleMaster.h"
#include "WalkerVehicles.generated.h"

class ACameraVehicle;
class UVehiclesAnimInstance;

UCLASS()
class LANDVEHICULE_API AWalkerVehicles : public AVehicleMaster
{
	GENERATED_BODY()

public:
	AWalkerVehicles();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	/*- Variables -*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta=(AllowPrivateAccess=true))
	USkeletalMeshComponent* SkeletalBaseVehicle;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta=(AllowPrivateAccess=true))
	TArray<ACameraVehicle*> Cameras;
	
	UPROPERTY()
	UVehiclesAnimInstance* AnimInstance;
	UPROPERTY()
	float CurrentAngle;

	/*- Function -*/
	UFUNCTION(BlueprintCallable)
	void SetTurretRotation(ACameraVehicle* CurrenCamera, float TurretAngle);
	UFUNCTION()
	void SetTurretElevation(float TurretElevation);
	UFUNCTION(BlueprintCallable)
	void SwitchToCamera(ACameraVehicle* NewCamera);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	ACameraVehicle* GeCameraInArray(int Index);

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetTurretAngle(ACameraVehicle* CurrenCamera, float InterpSpeed = 1.f);
};
