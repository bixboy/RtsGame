#include "Vehicles/VehiclePlayerMesh.h"


void UVehiclePlayerMesh::SetupPlayerMesh(USkeletalMesh* NewMesh)
{
	SetSkeletalMesh(NewMesh);
	SetVisibility(true);
	
	bIsUsed = true;

	Multicast_SetupPlayerMesh(NewMesh);
}

void UVehiclePlayerMesh::HidePlayerMesh()
{
	SetVisibility(false);
	bIsUsed = false;

	Multicast_HidePlayerMesh();
}

void UVehiclePlayerMesh::Multicast_SetupPlayerMesh_Implementation(USkeletalMesh* NewMesh)
{
	SetSkeletalMesh(NewMesh);
	SetVisibility(true);
}

void UVehiclePlayerMesh::Multicast_HidePlayerMesh_Implementation()
{
	SetVisibility(false);
}
