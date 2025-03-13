#include "Vehicles/Hover/HoverVehicles.h"
#include "Component/HoverPointComponent.h"
#include "Kismet/KismetSystemLibrary.h"


//--------------------------- Setup Functions ---------------------------
#pragma region Setup Functions

AHoverVehicles::AHoverVehicles()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	BaseVehicle->SetLinearDamping(0.5f);
	BaseVehicle->SetAngularDamping(0.5f);
}

void AHoverVehicles::BeginPlay()
{
	Super::BeginPlay();

	GetComponents(UHoverPointComponent::StaticClass(), HoverPoints, true);
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
		HandleManualRotation(DeltaTime);
	}
}

//--------------------------- Hovering ---------------------------
void AHoverVehicles::Hovering(float DeltaTime)
{
	if(EngineOn)
	{
		bool bIsOnGround = false;
		FVector AverageHoverForce = FVector::ZeroVector;
		int32 HoverPointCount = 0;

		for (const UHoverPointComponent* HoverPointComp : HoverPoints)
		{
			FHitResult HitResult;
			const float CurrentDistance = TraceGroundAtLocation(HoverPointComp, HitResult);

			if (HitResult.bBlockingHit)
			{
				bIsOnGround = true;

				const float DistanceError = HoverPointComp->HoverPoint.FloatingDistance - CurrentDistance;
				
				const float VelocityZ = BaseVehicle->GetPhysicsLinearVelocity().Z;
				
				const float SpringForce = DistanceError * HoverPointComp->HoverPoint.SpringStiffness;
				const float DampingForce = -VelocityZ * HoverPointComp->HoverPoint.DampingFactor;

				OscillationValue = FMath::FInterpTo(OscillationValue, FMath::Sin(GetWorld()->TimeSeconds * OscillationFrequency) * OscillationAmplitude, DeltaTime, OscillationSmoothing);

				const FVector SuspensionForce = FVector(0, 0, SpringForce + DampingForce + OscillationValue) ;

				Multicast_ApplyForceAtLocation(SuspensionForce, HoverPointComp->GetComponentLocation());

				AverageHoverForce += SuspensionForce;
				HoverPointCount++;
			}
        }

        if (bIsOnGround && HoverPointCount > 0)
        {
            AverageHoverForce /= HoverPointCount;
            for (const UHoverPointComponent* HoverPointComp : HoverPoints)
            {
                FVector HoverForce = AverageHoverForce;
            	BaseVehicle->AddImpulseAtLocation(HoverForce, HoverPointComp->GetComponentLocation());
            }
        }
        else if (!bIsOnGround)
        {
            // Si le véhicule n'est pas au sol, appliquez une force de gravité accrue
            FVector GravityForce = FVector(0, 0, GetWorld()->GetDefaultGravityZ() * 2.5f);
            for (const UHoverPointComponent* HoverPointComp : HoverPoints)
            {
                Multicast_ApplyForceAtLocation(GravityForce, HoverPointComp->GetComponentLocation());
            }
        }

		Movement(DeltaTime);
		Frictions();
	}
}

void AHoverVehicles::HandleManualRotation(float DeltaTime)
{
	FVector AngularVelocity = BaseVehicle->GetPhysicsAngularVelocityInDegrees();
	FVector DesiredTorque = FVector(0.f, 0.f, TurnInput * TurnForce);
	Multicast_AddTorque(DesiredTorque);

	FVector StabilizingTorque = -AngularVelocity * 4.f;
	Multicast_AddTorque(StabilizingTorque);
}

//--------------------------- Movement ---------------------------
#pragma region Movement

void AHoverVehicles::Movement(float DeltaTime)
{
/*--------------- Mouvement ---------------*/
    float CurrentSpeed = BaseVehicle->GetPhysicsLinearVelocity().Size();
    FVector ForwardVector = GetActorForwardVector();

    FVector Velocity = BaseVehicle->GetPhysicsLinearVelocity();
    FVector VelocityDirection = Velocity.GetSafeNormal();

    // Avancer
    if (ForwardInput > 0 && CurrentSpeed < MaxForwardSpeed)
    {
        FVector ForwardForce = ForwardVector * ForwardInput * MoveForce;
        Multicast_ApplyForce(ForwardForce);
        PlaySound(SoundMoveForward);
    }
    else if (ForwardInput < 0) // Reculer
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

    	FVector FrontLocation = HoverPoints[0]->GetComponentLocation();

    	float PitchForceMagnitude = FMath::Clamp(CurrentSpeed * BrakePitchMultiplier, 0.f, MaxBrakePitchForce);
    	FVector PitchForce = FVector(0.f, 0.f, PitchForceMagnitude);

    	Multicast_ApplyForceAtLocation(PitchForce, FrontLocation);
    }

    /*--------------- Rotation ---------------*/
	FVector Torque = FVector(0, 0, GetTurnInput() * TurnForce);
	Multicast_AddTorque(Torque);
	{
    	float SpeedFactor = FMath::Clamp(CurrentSpeed / MaxForwardSpeed, 0.f, 1.f);

    	float TargetTiltAngle = FMath::Clamp(GetTurnInput() * MaxTiltAngle * SpeedFactor, -MaxTiltAngle, MaxTiltAngle);
    	CurrentTiltAngle = FMath::FInterpTo(CurrentTiltAngle, TargetTiltAngle, DeltaTime, 3.f);

    	FRotator NewRotation = GetActorRotation();
    	NewRotation.Roll = CurrentTiltAngle;
    	UpdateVehicleRotation(NewRotation);
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
void AHoverVehicles::Multicast_ApplyForce_Implementation(const FVector Force)
{
	if(BaseVehicle)
		BaseVehicle->AddForce(Force, NAME_None, true);
}

void AHoverVehicles::Multicast_ApplyForceAtLocation_Implementation(const FVector Force, const FVector Location)
{
	if(BaseVehicle)
		BaseVehicle->AddImpulseAtLocation(Force, Location);
}

void AHoverVehicles::UpdateVehicleRotation(const FRotator& NewRotation)
{
	SetActorRotation(NewRotation);
}

void AHoverVehicles::Multicast_AddTorque_Implementation(const FVector& NewVector)
{
	if(BaseVehicle)
		BaseVehicle->AddTorqueInDegrees(NewVector, NAME_None, true);
}

#pragma endregion

// Trace Ground
float AHoverVehicles::TraceGroundAtLocation(const UHoverPointComponent* HoverPointComp, FHitResult& OutHitResult)
{
	float FloatingDistance;
	FVector StartLocation;
	FVector EndLocation;
	
	if (HoverPointComp)
	{
		FloatingDistance = HoverPointComp->HoverPoint.FloatingDistance;
		StartLocation =  HoverPointComp->GetComponentLocation();
		EndLocation = StartLocation + FVector(0, 0,  FloatingDistance * -2.f);	
	}
	else
	{
		FloatingDistance = 300.f;
		StartLocation =	GetActorLocation();
		EndLocation = StartLocation + FVector(0, 0,  FloatingDistance * -2.f);	
	}
	
	const bool bHit = UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		StartLocation,
		EndLocation,
		UEngineTypes::ConvertToTraceType(ECC_Camera),
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::ForOneFrame,
		OutHitResult,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		1.0f
	);

	if (bHit)
	{
		return OutHitResult.Distance;
	}

	return FloatingDistance * 2.f;
}

float AHoverVehicles::TraceGroundAtLocation(FHitResult& HitResult)
{
	return TraceGroundAtLocation(nullptr, HitResult);
}
