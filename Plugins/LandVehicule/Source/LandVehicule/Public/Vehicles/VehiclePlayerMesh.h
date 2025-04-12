#pragma once
#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "VehiclePlayerMesh.generated.h"


UCLASS()
class LANDVEHICULE_API UVehiclePlayerMesh : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void SetupPlayerMesh(USkeletalMesh* NewMesh);

	UFUNCTION()
	void HidePlayerMesh();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = settings, meta = (ClampMin = 1))
	int PlaceNumber = 1;

	UPROPERTY()
	bool bIsUsed;

protected:
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetupPlayerMesh(USkeletalMesh* NewMesh);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_HidePlayerMesh();

};
