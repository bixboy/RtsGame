#include "Vehicles/Hover/HoverVehicles.h"
#include "Kismet/KismetSystemLibrary.h"


//--------------------------- Setup Functions ---------------------------
#pragma region Setup Functions

AHoverVehicles::AHoverVehicles()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AHoverVehicles::BeginPlay()
{
	Super::BeginPlay();

	bReplicates = true;
	SetReplicateMovement(true); 
}

void AHoverVehicles::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

#pragma endregion

void AHoverVehicles::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (HasAuthority())
	{
		Hovering(DeltaTime);
	}
}

//--------------------------- Hovering ---------------------------
void AHoverVehicles::Hovering(float DeltaTime)
{
	if(EngineOn)
	{
		FHitResult HitResult;
		const float CurrentDistance = TraceGround(HitResult);
		const float DistanceError = FloatingDistance - CurrentDistance;
		
		const float SpringForce = DistanceError * SpringStiffness;
		const float DampingForce = -BaseVehicle->GetPhysicsLinearVelocity().Z * DampingFactor;
		OscillationValue = FMath::FInterpTo(OscillationValue, FMath::Sin(GetWorld()->TimeSeconds * OscillationFrequency) * OscillationAmplitude, DeltaTime, OscillationSmoothing);
		
		const FVector HoverForce = FVector(0, 0, SpringForce + DampingForce + OscillationValue + GetWorld()->GetDefaultGravityZ() * -1);
		Multicast_ApplyForce(HoverForce);

		if (HitResult.bBlockingHit)
		{
			FVector SurfaceNormal = HitResult.ImpactNormal;
			FRotator CurrentRotation = GetActorRotation();
			FRotator TargetRotation = FRotator(FRotationMatrix::MakeFromZ(SurfaceNormal).Rotator().Pitch, CurrentRotation.Yaw, CurrentRotation.Roll);
			
			FRotator SmoothedRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 2.f);
			Multicast_UpdateVehicleRotation(SmoothedRotation);
		}

		Movement(DeltaTime);
		Frictions();
	}
}

//--------------------------- Movement ---------------------------
#pragma region Movement

void AHoverVehicles::Movement(float DeltaTime)
{
	/*--------------- Movement ---------------*/
	float CurrentSpeed = BaseVehicle->GetPhysicsLinearVelocity().Size();
	FVector ForwardVector = GetActorForwardVector();

	FVector Velocity = BaseVehicle->GetPhysicsLinearVelocity();
	FVector VelocityDirection = Velocity.GetSafeNormal();

	// Forward
	if (ForwardInput > 0 && CurrentSpeed < MaxForwardSpeed)
	{
		FVector ForwardForce = ForwardVector * ForwardInput * MoveForce;
		Multicast_ApplyForce(ForwardForce);
		PlaySound(SoundMoveForward);
	}
	else if (ForwardInput < 0) //Backward
	{
		if (FVector::DotProduct(VelocityDirection, ForwardVector) > 0)
		{
			FVector BrakeForce = -VelocityDirection * BreakForce;
			Multicast_ApplyForce(BrakeForce);
			PlaySound(SoundBrake);
        
			if (CurrentSpeed < 20.f && CurrentSpeed < MaxReverseSpeed)
			{
				FVector ReverseForce = ForwardVector * ForwardInput * MoveForce * ReversMoveForceFactor;
				Multicast_ApplyForce(ReverseForce);
			}
		}
		else if (CurrentSpeed < MaxReverseSpeed)
		{
			FVector ReverseForce = ForwardVector * ForwardInput * MoveForce * ReversMoveForceFactor;
			Multicast_ApplyForce(ReverseForce);
		}
	}

	/*--------------- Rotation ---------------*/
	FVector Torque = FVector(0, 0, GetTurnInput() * TurnForce);
	Multicast_AddTorque(Torque);

	// Inclinaison
	if (FMath::Abs(TurnInput) > 0.1f)
	{
		float TargetTiltAngle = FMath::Clamp(TurnInput * MaxTiltAngle, -MaxTiltAngle, MaxTiltAngle);
		CurrentTiltAngle = FMath::FInterpTo(CurrentTiltAngle, TargetTiltAngle, DeltaTime, 3.f);

		FRotator NewRotation = GetActorRotation();
		NewRotation.Roll = CurrentTiltAngle;
		Multicast_UpdateVehicleRotation(NewRotation);
	}
	else
	{
		CurrentTiltAngle = FMath::FInterpTo(CurrentTiltAngle, 0.f, DeltaTime, 2.f);
		FRotator NewRotation = GetActorRotation();
		NewRotation.Roll = CurrentTiltAngle;
		Multicast_UpdateVehicleRotation(NewRotation);
	}
}

void AHoverVehicles::Frictions()
{
	// Movement Friction
	if (FMath::IsNearlyZero(ForwardInput))
	{
		FVector CurrentVelocity = BaseVehicle->GetPhysicsLinearVelocity();
		FVector FrictionForce = -CurrentVelocity * FrictionFactor;
		Multicast_ApplyForce(FrictionForce);
	}

	// Rotation Friction
	if (FMath::IsNearlyZero(TurnInput))
	{
		FVector CurrentAngularVelocity = BaseVehicle->GetPhysicsAngularVelocityInDegrees();

		FVector AngularFriction = -CurrentAngularVelocity * RotationFrictionFactor;
		Multicast_AddTorque(AngularFriction);
	}

	// Anti patinage
	FVector CurrentVelocity = BaseVehicle->GetPhysicsLinearVelocity();
	FVector ForwardVector = GetActorForwardVector();

	FVector LateralVelocity = CurrentVelocity - (FVector::DotProduct(CurrentVelocity, ForwardVector) * ForwardVector);

	FVector LateralFrictionForce = -LateralVelocity * RotationFrictionFactor;
	Multicast_ApplyForce(LateralFrictionForce);
}

//----------------------- Replication -----------------------
void AHoverVehicles::Multicast_ApplyForce_Implementation(const FVector& Force)
{
	if(BaseVehicle)
		BaseVehicle->AddForce(Force, NAME_None, true);
}

void AHoverVehicles::Multicast_UpdateVehicleRotation_Implementation(const FRotator& NewRotation)
{
	if (!HasAuthority()) return;
	
	SetActorRotation(NewRotation);
}

void AHoverVehicles::Multicast_AddTorque_Implementation(const FVector& NewVector)
{
	if(BaseVehicle)
		BaseVehicle->AddTorqueInDegrees(NewVector, NAME_None, true);
}

#pragma endregion

// Trace Ground
float AHoverVehicles::TraceGround(FHitResult& HitResult)
{
	const FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation + FVector(0, 0, FloatingDistance * -2.f);

	const bool bHit = UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		StartLocation,
		EndLocation,
		UEngineTypes::ConvertToTraceType(ECC_Camera),
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::ForOneFrame,
		HitResult,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		1.0f
	);

	if (bHit)
	{
		return HitResult.Distance;
	}
	
	return FloatingDistance * 2.f;
}
