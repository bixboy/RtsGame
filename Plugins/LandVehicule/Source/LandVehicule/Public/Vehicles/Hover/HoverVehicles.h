#pragma once
#include "CoreMinimal.h"
#include "Vehicles/VehicleMaster.h"
#include "HoverVehicles.generated.h"

UCLASS()
class LANDVEHICULE_API AHoverVehicles : public AVehicleMaster
{
	GENERATED_BODY()

public:
	AHoverVehicles();
	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void BeginPlay() override;

protected:
	/*- Vehicle Variables -*/
	UPROPERTY(EditAnywhere, Category = "Settings|Hover")
	float FloatingDistance = 150.f;
	UPROPERTY(EditAnywhere, Category = "Settings|Hover")
	float SpringStiffness = 150.0f;
	UPROPERTY(EditAnywhere, Category = "Settings|Hover")
	float DampingFactor = 10.0f;

	// Oscillation
	UPROPERTY(EditAnywhere, Category = "Settings|Hover|Oscillation")
	float OscillationFrequency = 3.0f; 
	UPROPERTY(EditAnywhere, Category = "Settings|Hover|Oscillation")
	float OscillationAmplitude = 2000.0f;
	UPROPERTY(EditAnywhere, Category = "Settings|Hover|Oscillation")
	float OscillationSmoothing = 10.0f;

	// Move Force
	UPROPERTY(EditAnywhere, Category = "Settings|Hover Movement|Speed")
	float MoveForce = 2000.0f;
	UPROPERTY(EditAnywhere, Category = "Settings|Hover Movement|Speed")
	float ReversMoveForceFactor = 0.5f;

	// Speed
	UPROPERTY(EditAnywhere, Category = "Settings|Hover Movement|Speed")
	float MaxForwardSpeed = 5000.f;
	UPROPERTY(EditAnywhere, Category = "Settings|Hover Movement|Speed")
	float MaxReverseSpeed = 200.f;

	UPROPERTY(EditAnywhere, Category = "Settings|Hover Movement|Speed")
	float BreakForce = 1250.f;

	// Rotation
	UPROPERTY(EditAnywhere, Category = "Settings|Hover Movement|Turn")
	float TurnForce = 50.0f;
	UPROPERTY(EditAnywhere, Category = "Settings|Hover Movement|Turn")
	float MaxTiltAngle = 30.f;

	// Friction
	UPROPERTY(EditAnywhere, Category = "Settings|Hover Movement|Speed")
	float FrictionFactor = 2.f;
	UPROPERTY(EditAnywhere, Category = "Settings|Hover Movement|Turn")
	float RotationFrictionFactor = 2.f;

	UPROPERTY()
	float OscillationValue = 0.0f;
	
	UPROPERTY()
	float CurrentTiltAngle = 0.0f;
	
	/*- Function -*/
	UFUNCTION()
	void Hovering(float DeltaTime);
	
	UFUNCTION()
	void Movement(float DeltaTime);

	UFUNCTION()
	void Frictions();
	
	UFUNCTION()
	float TraceGround(FHitResult& HitResult);
	
	// Replication
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ApplyForce(const FVector& Force);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpdateVehicleRotation(const FRotator& NewRotation);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_AddTorque(const FVector& NewVector);
	
};
