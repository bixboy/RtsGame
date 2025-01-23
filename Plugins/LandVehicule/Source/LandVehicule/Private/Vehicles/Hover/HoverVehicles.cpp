#include "Vehicles/Hover/HoverVehicles.h"
#include "Kismet/KismetSystemLibrary.h"

AHoverVehicles::AHoverVehicles()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AHoverVehicles::BeginPlay()
{
	Super::BeginPlay();
}

void AHoverVehicles::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	Hovering(DeltaTime);
}

void AHoverVehicles::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

/*- Hovering -*/
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
		BaseVehicle->AddForce(HoverForce, NAME_None, true);

		if (HitResult.bBlockingHit)
		{
			FVector SurfaceNormal = HitResult.ImpactNormal;
			FRotator CurrentRotation = GetActorRotation();
			FRotator TargetRotation = FRotator(FRotationMatrix::MakeFromZ(SurfaceNormal).Rotator().Pitch, CurrentRotation.Yaw, CurrentRotation.Roll);
			
			FRotator SmoothedRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 2.f);
			SetActorRotation(SmoothedRotation);
		}

		Movement(DeltaTime);
		Frictions();
	}
}

// Movement
#pragma region Movement

void AHoverVehicles::Movement(float DeltaTime)
{
	/*- Movement -*/
	float CurrentSpeed = BaseVehicle->GetPhysicsLinearVelocity().Size();
	FVector ForwardVector = GetActorForwardVector();

	FVector Velocity = BaseVehicle->GetPhysicsLinearVelocity();
	FVector VelocityDirection = Velocity.GetSafeNormal();

	// Forward
	if (ForwardInput > 0 && CurrentSpeed < MaxForwardSpeed)
	{
		FVector ForwardForce = ForwardVector * ForwardInput * MoveForce;
		BaseVehicle->AddForce(ForwardForce, NAME_None, true);
		PlaySound(SoundMoveForward);
	}
	else if (ForwardInput < 0) //Backward
	{
		if (FVector::DotProduct(VelocityDirection, ForwardVector) > 0)
		{
			FVector BrakeForce = -VelocityDirection * BreakForce;
			BaseVehicle->AddForce(BrakeForce, NAME_None, true);
			PlaySound(SoundBrake);
        
			if (CurrentSpeed < 20.f && CurrentSpeed < MaxReverseSpeed)
			{
				FVector ReverseForce = ForwardVector * ForwardInput * MoveForce * ReversMoveForceFactor;
				BaseVehicle->AddForce(ReverseForce, NAME_None, true);
			}
		}
		else if (CurrentSpeed < MaxReverseSpeed)
		{
			FVector ReverseForce = ForwardVector * ForwardInput * MoveForce * ReversMoveForceFactor;
			BaseVehicle->AddForce(ReverseForce, NAME_None, true);
		}
	}

	/*- Rotation -*/
	FVector Torque = FVector(0, 0, GetTurnInput() * TurnForce);
	BaseVehicle->AddTorqueInDegrees(Torque, NAME_None, true);

	// Inclinaison
	if (FMath::Abs(TurnInput) > 0.1f)
	{
		float TargetTiltAngle = FMath::Clamp(TurnInput * MaxTiltAngle, -MaxTiltAngle, MaxTiltAngle);
		CurrentTiltAngle = FMath::FInterpTo(CurrentTiltAngle, TargetTiltAngle, DeltaTime, 3.f);

		FRotator NewRotation = GetActorRotation();
		NewRotation.Roll = CurrentTiltAngle;
		SetActorRotation(NewRotation);
	}
	else
	{
		CurrentTiltAngle = FMath::FInterpTo(CurrentTiltAngle, 0.f, DeltaTime, 2.f);
		FRotator NewRotation = GetActorRotation();
		NewRotation.Roll = CurrentTiltAngle;
		SetActorRotation(NewRotation);
	}
}

void AHoverVehicles::Frictions()
{
	// Movement Friction
	if (FMath::IsNearlyZero(ForwardInput))
	{
		FVector CurrentVelocity = BaseVehicle->GetPhysicsLinearVelocity();
		FVector FrictionForce = -CurrentVelocity * FrictionFactor; 
		BaseVehicle->AddForce(FrictionForce, NAME_None, true);
	}

	// Rotation Friction
	if (FMath::IsNearlyZero(TurnInput))
	{
		FVector CurrentAngularVelocity = BaseVehicle->GetPhysicsAngularVelocityInDegrees();

		FVector AngularFriction = -CurrentAngularVelocity * RotationFrictionFactor;
		BaseVehicle->AddTorqueInDegrees(AngularFriction, NAME_None, true);
	}

	// Anti patinage
	FVector CurrentVelocity = BaseVehicle->GetPhysicsLinearVelocity();
	FVector ForwardVector = GetActorForwardVector();

	FVector LateralVelocity = CurrentVelocity - (FVector::DotProduct(CurrentVelocity, ForwardVector) * ForwardVector);

	FVector LateralFrictionForce = -LateralVelocity * RotationFrictionFactor;
	BaseVehicle->AddForce(LateralFrictionForce, NAME_None, true);
}

#pragma endregion

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
		FLinearColor::Red,                // Couleur du debug si pas d'impact
		FLinearColor::Green,              // Couleur du debug si impact
		1.0f                              // Durée d'affichage du debug
	);

	if (bHit)
	{
		return HitResult.Distance;
	}
	
	return FloatingDistance * 2.f;
}
