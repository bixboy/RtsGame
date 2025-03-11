#pragma once
#include "CoreMinimal.h"
#include "VehicleData.generated.h"

class ACameraVehicle;


UENUM(BlueprintType)
enum class EVehiclePlaceType : uint8
{
	Driver,
	Gunner,
	None
};

USTRUCT(BlueprintType)
struct FVehicleRole
{
	GENERATED_BODY()

	UPROPERTY()
	APawn* Player = nullptr;

	UPROPERTY()
	EVehiclePlaceType RoleName;

	bool operator==(const FVehicleRole& Other) const
	{
		return	Player == Other.Player &&
				RoleName == Other.RoleName;
	}
};

USTRUCT(BlueprintType)
struct FTurrets
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	ACameraVehicle* CameraVehicle = nullptr;

	UPROPERTY(BlueprintReadWrite)
	APlayerController* PlayerOwner = nullptr;

	UPROPERTY(BlueprintReadWrite)
	float AccumulatedYaw = 0.f;
	
	UPROPERTY(BlueprintReadWrite)
	float AccumulatedPitch = 0.f;
	
	bool operator==(const FTurrets& Other) const
	{
		return CameraVehicle == Other.CameraVehicle && 
			   AccumulatedYaw == Other.AccumulatedYaw && 
			   AccumulatedPitch == Other.AccumulatedPitch &&
			   PlayerOwner == Other.PlayerOwner;
	}
};
