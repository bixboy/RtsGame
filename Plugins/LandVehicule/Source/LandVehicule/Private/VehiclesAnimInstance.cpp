#include "VehiclesAnimInstance.h"

void UVehiclesAnimInstance::UpdateTurretRotation(FRotator NewAngle, FName ParentSocketName)
{
	if (!TurretAngle.Contains(ParentSocketName))
	{
		TurretAngle.Add(ParentSocketName, NewAngle);
	}
	else
	{
		TurretAngle[ParentSocketName] = NewAngle;
	}

}

FRotator UVehiclesAnimInstance::GetTurretRotation(FName TurretName) const
{
	if (TurretAngle.IsEmpty() || !TurretAngle.Contains(TurretName)) return FRotator::ZeroRotator;
	
	return TurretAngle[TurretName];
}


