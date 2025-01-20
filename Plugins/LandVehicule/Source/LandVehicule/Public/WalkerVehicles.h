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

protected:
	virtual void BeginPlay() override;

private:
	/*- Variables -*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta=(AllowPrivateAccess=true))
	USkeletalMeshComponent* SkeletalBaseVehicle;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta=(AllowPrivateAccess=true))
	TArray<ACameraVehicle*> Cameras;
	UPROPERTY()
	ACameraVehicle* CurrentCamera;
	
	UPROPERTY()
	UVehiclesAnimInstance* AnimInstance;
	
	FRotator CurrentAngle;
	float AccumulatedYaw = 0.0f;
	float AccumulatedPitch = 0.0f;

	/*- Function -*/
	UFUNCTION(BlueprintCallable)
	void SetTurretRotation(ACameraVehicle* CurrenCamera, FRotator TurretAngle);
	UFUNCTION()
	void SetTurretElevation(float TurretElevation);
	UFUNCTION(BlueprintCallable)
	void SwitchToCamera(ACameraVehicle* NewCamera);
	UFUNCTION(BlueprintCallable)
	void SwitchToNextCamera();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	ACameraVehicle* GeCameraInArray(int Index);
	UFUNCTION(BlueprintCallable)
	void ApplyCameraRotation(float DeltaYaw, float DeltaPitch);

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FRotator GetTurretAngle(ACameraVehicle* CurrenCamera, float InterpSpeed = 1.f);
};
