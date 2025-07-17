#include "Component/VehicleCamera.h"


UVehicleCamera::UVehicleCamera()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UVehicleCamera::BeginPlay()
{
	Super::BeginPlay();
}
