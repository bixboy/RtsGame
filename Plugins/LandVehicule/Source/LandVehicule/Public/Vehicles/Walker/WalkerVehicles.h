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

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void WalkerMove(FVector2D Direction);
	UFUNCTION(BlueprintCallable)
	void WalkerStopMove();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Walker Movement")
	float MovementSpeed = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Walker Movement")
	UAnimMontage* WalkerMovementAnim;
};
