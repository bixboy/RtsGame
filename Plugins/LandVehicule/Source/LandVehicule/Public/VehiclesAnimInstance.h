#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "VehiclesAnimInstance.generated.h"

class UCameraComponent;

UCLASS()
class LANDVEHICULE_API UVehiclesAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void UpdateTurretRotation(float NewAngle, FName ParentSocketName);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<FName, float> TurretAngle;
};
