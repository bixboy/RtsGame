#pragma once
#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Components/ActorComponent.h"
#include "VehicleCamera.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LANDVEHICULE_API UVehicleCamera : public UCameraComponent
{
	GENERATED_BODY()

public:
	UVehicleCamera();

	UPROPERTY(EditAnywhere, Category = "Settings")
	bool bIsTurrets = true;

	UPROPERTY(EditAnywhere, Category = "Settings")
	bool bSwitchToOtherTypeCam = true;

protected:
	virtual void BeginPlay() override;
};
