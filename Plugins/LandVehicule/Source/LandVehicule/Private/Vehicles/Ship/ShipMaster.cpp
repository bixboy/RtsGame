#include "Vehicles/Ship/ShipMaster.h"
#include "EnhancedInputComponent.h"
#include "Net/UnrealNetwork.h"


#pragma region Setup

AShipMaster::AShipMaster()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AShipMaster::BeginPlay()
{
	Super::BeginPlay();

	if (BaseVehicle)
	{
		BaseVehicle->SetSimulatePhysics(true);
		BaseVehicle->SetAngularDamping(1.f);
		BaseVehicle->SetEnableGravity(true);
	}

	if (HasAuthority())
	{
		OnVehicleMove.AddDynamic(this, &AShipMaster::OnShipMove);
		
		LandingGears.Empty();
		TArray<USceneComponent*> AllComps;
		GetComponents<USceneComponent>(AllComps);

		for (USceneComponent* Comp : AllComps)
		{
			if (Comp->ComponentHasTag(TEXT("LandingGear")))
			{
				LandingGears.Add(Comp);
			}
		}
	}
}

void AShipMaster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (auto* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInput->BindAction(BoostAction, ETriggerEvent::Started, this, &AShipMaster::Input_OnBoost);

		EnhancedInput->BindAction(ThrustAction, ETriggerEvent::Triggered, this, &AShipMaster::Input_Thrust);
		EnhancedInput->BindAction(ThrustAction, ETriggerEvent::Completed, this, &AShipMaster::Input_Thrust);

		EnhancedInput->BindAction(PivoAction, ETriggerEvent::Triggered, this, &AShipMaster::Server_OnShipYaw);
		EnhancedInput->BindAction(LiftAction, ETriggerEvent::Triggered, this, &AShipMaster::Server_OnShipLift);

		EnhancedInput->BindAction(LandingAction, ETriggerEvent::Started, this, &AShipMaster::AttemptLanding);
	}
}

void AShipMaster::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShipMaster, bSuperSpeed);
}

#pragma endregion


void AShipMaster::Server_SwitchEngine(bool OnOff)
{
	Super::Server_SwitchEngine(OnOff);
}


void AShipMaster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority() || !BaseVehicle || !bEngineOn || bIsLanded)
	return;

	// Check Is in Landing
	if (bIsLanding)
	{
		ShipLanding(DeltaTime);
		return;
	}

	if (ThrustInput != 0 && bHoverActive)
	{
		bHoverActive = false;
		BaseVehicle->SetEnableGravity(false);
	}

	// ——————————————————————————————————————————————————————————————————
    // 0) HOVER
    FVector HoverForce = FVector::ZeroVector;
    if (bHoverActive)
    {
        const FVector Start = BaseVehicle->GetComponentLocation();
        const FVector End   = Start - FVector::UpVector * (HoverHeight * 2.f);

        FHitResult Hit;
        FCollisionQueryParams QP(NAME_None, false, this);
    	
        if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, QP))
        {
            float CurrentDist = Start.Z - Hit.Location.Z;
            float Error = HoverHeight - CurrentDist;

            if (FMath::Abs(Error) < 10.f)
            {
            	BaseVehicle->SetEnableGravity(false);
                bHoverActive = false;
            }
            else
            {
                float DynamicStiff = HoverStiffness * (1.f + FMath::Clamp(FMath::Abs(Error) / HoverHeight, 0.f, 2.f));
                float VelUp = FVector::DotProduct(BaseVehicle->GetPhysicsLinearVelocity(), FVector::UpVector);

                float SpringAccel = DynamicStiff * Error;
                float DampingAccel = HoverDamping * (-VelUp);
                float TotalAccel = (SpringAccel + DampingAccel) / BaseVehicle->GetMass();

                HoverForce = FVector::UpVector * (TotalAccel * BaseVehicle->GetMass());
            }
        }

    	BaseVehicle->AddForce(HoverForce);
    	return;
    }

    // ——————————————————————————————————————————————————————————————————
	// 1) Thrust
	if (!bHoverActive)
	{
		float TargetThrust = Thrust;
		if (ThrustInput >  KINDA_SMALL_NUMBER)
		{
			TargetThrust += ThrustIncreaseRate * DeltaTime;	
		}
		else if (ThrustInput < -KINDA_SMALL_NUMBER)
		{
			TargetThrust -= ThrustDecreaseRate * DeltaTime;	
		}

		TargetThrust = FMath::Clamp(TargetThrust, 0.f, MaxThrust);
		Thrust = FMath::FInterpTo(Thrust, TargetThrust, DeltaTime, ThrustInterpSpeed);

		
		// 2) Speed Factor
		FVector ForwardDir = BaseVehicle->GetForwardVector();
		FVector ForwardVel = ForwardDir * Thrust;

		float PitchDot = FVector::DotProduct(ForwardDir, FVector::UpVector);
		float AbsDot = FMath::Abs(PitchDot);

		float SpeedFactor;

		if (PitchDot > 0.f)
		{
			SpeedFactor = FMath::Lerp(1.f, MinUpSpeedFactor, AbsDot);
		}
		else
		{
			SpeedFactor = FMath::Lerp(1.f, MaxDownSpeedFactor, AbsDot);
		}

		ForwardVel *= SpeedFactor;

		
		// 4) fallback gravity
		FVector AppliedUp = FVector::ZeroVector;
		if (ForwardVel.Size() < GravityThreshold)
		{
			float Alpha = ForwardVel.Size() / GravityThreshold;
			AppliedUp = FMath::Lerp(0.f, -980.f, 1.f - Alpha) * BaseVehicle->GetUpVector();
		}
		else
		{
			AppliedUp = FMath::VInterpTo(AppliedUp, FVector::ZeroVector, DeltaTime, 20.f);
		}

		if (bSuperSpeed)
		{
			ForwardVel *= BoostMultiplier;
		}

		// 5) Apply Velocity
		FVector CurrVel = BaseVehicle->GetPhysicsLinearVelocity();
		CurrVel.X = ForwardVel.X;
		CurrVel.Y = ForwardVel.Y;
		CurrVel.Z = ForwardVel.Z + AppliedUp.Z;
		BaseVehicle->SetPhysicsLinearVelocity(CurrVel);
	}
}


// ======= Movements =======
#pragma region Movements

void AShipMaster::Input_Thrust(const FInputActionValue& InputActionValue)
{
	Server_Thrust(InputActionValue);
}

void AShipMaster::Server_Thrust_Implementation(FInputActionValue InputActionValue)
{
	if (!bEngineOn) return;
	
	ThrustInput = InputActionValue.Get<float>();
}

void AShipMaster::OnShipMove(float NewForwardInput, float NewRightInput)
{
	if (!bEngineOn || bIsLanding || bIsLanded) return;
	
	// ====== Pitch Up / Down ======
	if (NewForwardInput != 0.f)
	{
		FVector NewTorque = (NewForwardInput * ForwardTorquePower) * BaseVehicle->GetRightVector();
		BaseVehicle->AddTorqueInDegrees(NewTorque, "None", true);
	}

	// ====== Roll Left / Roll Right ======
	if (FMath::Abs(NewRightInput) > KINDA_SMALL_NUMBER)
	{

		float SpeedFactor = FMath::Clamp(Thrust / MaxThrust, 0.f, 1.f);
		float RollFactor = FMath::Lerp(1.f, MaxRollSpeedMultiplier, SpeedFactor);
		
		float TorquePower = ForwardTorquePower * RollFactor;

		FVector NewTorque = (-NewRightInput * TorquePower) * BaseVehicle->GetForwardVector();
		BaseVehicle->AddTorqueInDegrees(NewTorque, NAME_None, true);
	}
}

void AShipMaster::Server_OnShipYaw_Implementation(const FInputActionValue& InputActionValue)
{
	if (!bEngineOn || bIsLanding || bIsLanded || !BaseVehicle) return;
	
	float YawInput = InputActionValue.Get<float>();
	
	// ====== Pivo Left / Right ======
	FVector Torque = BaseVehicle->GetUpVector() * (YawInput * SideTorquePower);
	BaseVehicle->AddTorqueInDegrees(Torque, NAME_None, true);
}

void AShipMaster::Server_OnShipLift_Implementation(const FInputActionValue& InputActionValue)
{
	if (!bEngineOn || !BaseVehicle || bIsLanding || bIsLanded) return;

	float LiftInput = InputActionValue.Get<float>();
	
	// ====== Up / Down ======
	FVector Force = FVector::UpVector * (LiftInput * LiftForcePower);
	BaseVehicle->AddForce(Force, NAME_None, true);
}

#pragma endregion


// ======= Boost =======
#pragma region Boost

void AShipMaster::Input_OnBoost()
{
	Server_OnBoost();
}

void AShipMaster::Server_OnBoost_Implementation()
{
	if (bSuperSpeed)
	{
		GetWorldTimerManager().ClearTimer(BoostTimerHandle);
	}

	bSuperSpeed = true;

	GetWorldTimerManager().SetTimer(BoostTimerHandle, this, &AShipMaster::EndBoost, BoostDuration, false);
}

void AShipMaster::EndBoost()
{
	bSuperSpeed = false;
}

#pragma endregion


// ======= Landing =======
#pragma region Landing

void AShipMaster::TakeOff()
{
	if (!BaseVehicle)
		return;

	BaseVehicle->SetSimulatePhysics(true);
	BaseVehicle->SetEnableGravity(false);

	bIsLanding = false;
	
	if (bIsLanded)
	{
		bIsLanded = false;
		bHoverActive = true;
		
		BaseVehicle->SetEnableGravity(true);
	}

	bEngineOn = true;
}

void AShipMaster::AttemptLanding()
{
	if (!BaseVehicle)
		return;

	if (bIsLanded || bIsLanding)
	{
		TakeOff();
		return;
	}

	if (bEngineOn && Thrust <= MaxThrust * LandingMaxThrustFactor)
	{
		FVector Start = BaseVehicle->GetComponentLocation();
		FVector End = Start - FVector::UpVector * LandingDistanceThreshold * 1.5f;

		FVector Forward = BaseVehicle->GetForwardVector();
		FQuat OrientationQuat = FRotationMatrix::MakeFromX(Forward).ToQuat();

		FHitResult OutHit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		bool bHit = GetWorld()->SweepSingleByChannel(
			OutHit,
			Start,
			End,
			FRotator::ZeroRotator.Quaternion(),
			ECC_GameTraceChannel5,
			FCollisionShape::MakeBox( BaseVehicle->GetCollisionShape().GetExtent()),
			Params
		);

		float Dist = (Start.Z - OutHit.Location.Z);
		if (bHit && Dist <= LandingDistanceThreshold)
		{
			bIsLanding = true;
			bIsLanded = false;
			bHoverActive = false;
			
			Thrust = 0.f;
			ThrustInput = 0.f;

			BaseVehicle->SetSimulatePhysics(true);
			BaseVehicle->SetEnableGravity(false);
			BaseVehicle->SetPhysicsLinearVelocity(FVector::ZeroVector);

			InitialRotation = GetActorRotation();
			LandingStartZ = GetActorLocation().Z;
			
			TotalLandingDistance = LandingStartZ - (OutHit.Location.Z + LegContactDistance);
		}
	}
}

void AShipMaster::ShipLanding(float DeltaTime)
{
	if (ThrustInput > KINDA_SMALL_NUMBER)
    {
        TakeOff();
        return;
    }

    // --- Descente verticale continue ---
    FVector Loc = GetActorLocation();
    float CurrZ = Loc.Z;

    float DescStep = LandingSpeed * DeltaTime;
    float TentativeZ = CurrZ - DescStep;
	Loc.Z = TentativeZ;
	
    SetActorLocation(Loc);

    // --- Calcul ---
    float DoneDist = LandingStartZ - Loc.Z;
    float Alpha = (TotalLandingDistance > KINDA_SMALL_NUMBER) ?
    	FMath::Clamp(DoneDist / TotalLandingDistance, 0.f, 1.f): 1.f;

    // --- Take Normal ---
    FVector Pivot = BaseVehicle->GetComponentLocation();
    FVector End = Pivot - FVector::UpVector * (LandingDistanceThreshold * 2.f);
    FVector GroundNormal = FVector::UpVector;

	FHitResult Hit;
    if (GetWorld()->LineTraceSingleByChannel(Hit, Pivot, End, ECC_GameTraceChannel5, FCollisionQueryParams(NAME_None, false, this)))
    {
        GroundNormal = Hit.Normal.GetSafeNormal();
    }

    // --- Rotation ---
	{
		FVector Forward = BaseVehicle->GetForwardVector();
		FVector TangentF = FVector::VectorPlaneProject(Forward, GroundNormal).GetSafeNormal();
		FRotator GroundAlignRot = FRotationMatrix::MakeFromXZ(TangentF, GroundNormal).Rotator();

		FQuat Q0 = InitialRotation.Quaternion();
		FQuat Q1 = GroundAlignRot.Quaternion();
		FQuat Qn = FQuat::Slerp(Q0, Q1, Alpha);
	
		SetActorRotation(Qn);	
	}

    // --- Landing Gears ---
	{
		bool bAllDown = true;
		for (int32 i = 0; i < LandingGears.Num(); ++i)
		{
			USceneComponent* Leg = LandingGears[i];
			FVector A = Leg->GetComponentLocation();
			FVector B = A - GroundNormal * LegContactDistance;
        
			FHitResult LegHit;
			if (!GetWorld()->LineTraceSingleByChannel(LegHit, A, B, ECC_GameTraceChannel5, FCollisionQueryParams(NAME_None, false, this)))
			{
				bAllDown = false;
			}
		}
		if (bAllDown)
		{
			bIsLanding = false;
			bIsLanded  = true;
			BaseVehicle->SetSimulatePhysics(false);
		}	
	}
}

#pragma endregion
