#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "VehiclesInteractions.generated.h"

class ACameraVehicle;

UINTERFACE()
class UVehiclesInteractions : public UInterface
{
	GENERATED_BODY()
};

class LANDVEHICULE_API IVehiclesInteractions
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool Interact(APawn* PlayerInteract);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void UpdateTurretRotation(FVector2D Rotation, FName TurretName);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	ACameraVehicle* GetCurrentCameraVehicle();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ChangePlace(APawn* Player);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OutOfVehicle(APawn* Player);
};
