#include "Vehicles/Ship/ShipMaster.h"
#include "EnhancedInputComponent.h"
#include "Kismet/KismetMathLibrary.h"
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
		
		//Thrust = MaxThrust / 2;
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

	if (!OnOff)
	{
		BaseVehicle->SetEnableGravity(true);
		bHoverActive = false;
	}
	else
	{
		ThrustInput = 0.f;
		bHoverActive = true;
	}
}


void AShipMaster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority() || !BaseVehicle || !bEngineOn)
	return;

	if (ThrustInput != 0)
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
                float VelUp        = FVector::DotProduct(BaseVehicle->GetPhysicsLinearVelocity(), FVector::UpVector);

                float SpringAccel  = DynamicStiff * Error;
                float DampingAccel = HoverDamping * (-VelUp);
                float TotalAccel   = (SpringAccel + DampingAccel) / BaseVehicle->GetMass();

                HoverForce = FVector::UpVector * (TotalAccel * BaseVehicle->GetMass());
            }
        }
    }

    // ——————————————————————————————————————————————————————————————————
	if (!bHoverActive)
	{
		// 1) Thrust
		float TargetThrust = Thrust;
		if (ThrustInput > KINDA_SMALL_NUMBER)
		{
			TargetThrust = Thrust + ThrustIncreaseRate * DeltaTime;   
		}
		else if (ThrustInput < -KINDA_SMALL_NUMBER)
		{
			TargetThrust = Thrust - ThrustDecreaseRate * DeltaTime;   
		}

		TargetThrust = FMath::Clamp(TargetThrust, 0.f, MaxThrust);
		Thrust = FMath::FInterpTo(Thrust, TargetThrust, DeltaTime, ThrustInterpSpeed);
		
		// ——————————————————————————————————————————————————————————————————
		// 2) Gravité
		FVector ForwardVel = BaseVehicle->GetForwardVector() * Thrust;
		FVector AppliedUp = FVector::ZeroVector;
		
		if (ForwardVel.Size() < GravityThreshold)
		{
			float Alpha = ForwardVel.Size() / GravityThreshold;
			AppliedUp   = FMath::Lerp(0.f, -980.f, 1.f - Alpha) * BaseVehicle->GetUpVector();
		}
		else
		{
			AppliedUp = FMath::VInterpTo(AppliedUp, FVector::ZeroVector, DeltaTime, 20.f);
		}

		// ——————————————————————————————————————————————————————————————————
		FVector FinalVel = ForwardVel + AppliedUp;
		BaseVehicle->SetPhysicsLinearVelocity(FinalVel);
}

    if (bHoverActive)
    {
        BaseVehicle->AddForce(HoverForce);
    }
}

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
	if (!bEngineOn) return;
	
	// ====== Up / Down ======
	if (NewForwardInput != 0.f)
	{
		FVector NewTorque = (NewForwardInput * ForwardTorquePower) * BaseVehicle->GetRightVector();
		BaseVehicle->AddTorqueInDegrees(NewTorque, "None", true);
	}

	// ====== Roll Left / Roll Right ======
	if (NewRightInput != 0.f)
	{
		FVector NewTorque = (-NewRightInput * ForwardTorquePower) * BaseVehicle->GetForwardVector();
		BaseVehicle->AddTorqueInDegrees(NewTorque, "None", true);
	}
}

void AShipMaster::Input_OnBoost()
{
	Server_OnBoost();
}

void AShipMaster::Server_OnBoost_Implementation()
{
	bSuperSpeed = !bSuperSpeed;
}

