#include "VehiclesAnimInstance.h"

void UVehiclesAnimInstance::UpdateTurretRotation(float NewAngle, FName ParentSocketName)
{
	TurretAngle[ParentSocketName] = NewAngle;
}
