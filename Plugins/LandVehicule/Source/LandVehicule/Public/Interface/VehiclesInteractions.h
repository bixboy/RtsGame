#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "VehiclesInteractions.generated.h"

class ACustomPlayerController;
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
	bool Interact(ACustomPlayerController* PlayerInteract);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OutOfVehicle(ACustomPlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ChangePlace(ACustomPlayerController* Player);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void UpdateTurretRotation(FVector2D Rotation, FName TurretName);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	ACameraVehicle* GetCurrentCameraVehicle();
	
};
