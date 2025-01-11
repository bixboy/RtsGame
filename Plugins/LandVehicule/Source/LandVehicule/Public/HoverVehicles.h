#pragma once

#include "CoreMinimal.h"
#include "VehicleMaster.h"
#include "HoverVehicles.generated.h"

UCLASS()
class LANDVEHICULE_API AHoverVehicles : public AVehicleMaster
{
	GENERATED_BODY()

public:
	AHoverVehicles();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

private:
	/*- Vehicle Variables -*/
	UPROPERTY(EditAnywhere, Category = "Settings|Hover")
	float FloatingDistance = 150.f;
	UPROPERTY(EditAnywhere, Category = "Settings|Hover")
	float SpringStiffness = 150.0f;
	UPROPERTY(EditAnywhere, Category = "Settings|Hover")
	float DampingFactor = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Settings|Hover|Oscillation")
	float OscillationFrequency = 3.0f; 
	UPROPERTY(EditAnywhere, Category = "Settings|Hover|Oscillation")
	float OscillationAmplitude = 2000.0f;
	UPROPERTY(EditAnywhere, Category = "Settings|Hover|Oscillation")
	float OscillationSmoothing = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Settings|Hover Movement|Speed")
	float MoveForce = 2000.0f;
	UPROPERTY(EditAnywhere, Category = "Settings|Hover Movement|Speed")
	float ReversMoveForceFactor = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Settings|Hover Movement|Speed")
	float MaxForwardSpeed = 5000.f;
	UPROPERTY(EditAnywhere, Category = "Settings|Hover Movement|Speed")
	float MaxReverseSpeed = 200.f;

	UPROPERTY(EditAnywhere, Category = "Settings|Hover Movement|Speed")
	float BreakForce = 1250.f;
	
	UPROPERTY(EditAnywhere, Category = "Settings|Hover Movement|Turn")
	float TurnForce = 50.0f;
	UPROPERTY(EditAnywhere, Category = "Settings|Hover Movement|Turn")
	float MaxTiltAngle = 30.f;

	UPROPERTY(EditAnywhere, Category = "Settings|Hover Movement|Speed")
	float FrictionFactor = 2.f;
	UPROPERTY(EditAnywhere, Category = "Settings|Hover Movement|Turn")
	float RotationFrictionFactor = 2.f;

	float OscillationValue = 0.0f;
	float CurrentTiltAngle = 0.0f;
	
	/*- Function -*/
	UFUNCTION()
	void Hovering(float DeltaTime);
	UFUNCTION()
	void Movement(float DeltaTime);
	UFUNCTION()
	void Frictions();
	
	UFUNCTION()
	float TraceGround();
	
};
