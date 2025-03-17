#pragma once

#include "CoreMinimal.h"
#include "Vehicles/VehicleMaster.h"
#include "WalkerVehicles.generated.h"

class ACameraVehicle;
class UVehiclesAnimInstance;

UCLASS()
class LANDVEHICULE_API AWalkerVehicles : public AVehicleMaster
{
	GENERATED_BODY()

public:
	AWalkerVehicles();

	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void WalkerMoving(float FowardInput, float RightInput);

	UFUNCTION()
	void UpdateLegsPosition();

	UFUNCTION()
	float GetLegHeight(float Distance);

	UFUNCTION()
	FVector GetCenterOfWalker(FVector Location, float Upper, float Lower, FHitResult& OutHit, float Radius = 30.f);
	FVector GetCenterOfWalker(FVector Start, FVector End, FHitResult& OutHit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Walker Movement")
	float MaxStep = 40.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Walker Movement")
	float StepHeight = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Walker Movement")
	float FootHeight = 180.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Walker Movement")
	UAnimMontage* WalkerMovementAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Walker Movement|Control Rig")
	TArray<FAnimationLeg> Legs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Walker Movement|Control Rig")
	FVector WalkerCenter;

	UPROPERTY(BlueprintReadWrite)
	bool bLegsIsReady = false;

	UPROPERTY()
	FVector LastLocation;
};
