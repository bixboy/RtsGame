#include "Vehicles/Walker/WalkerVehicles.h"
#include "CameraVehicle.h"
#include "VehiclesAnimInstance.h"

AWalkerVehicles::AWalkerVehicles()
{
	PrimaryActorTick.bCanEverTick = true;

	SkeletalBaseVehicle = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalBaseVehicle");
	SkeletalBaseVehicle->SetupAttachment(RootComponent);
}

void AWalkerVehicles::BeginPlay()
{
	Super::BeginPlay();
	
	SwitchToNextCamera(Cast<APlayerController>(GetController()));
}

void AWalkerVehicles::ApplyTurretRotation(float DeltaYaw, float DeltaPitch)
{
	if (CurrentCamera && SkeletalBaseVehicle)
	{
		FRotator NewCameraRotation = CurrentCamera->GetCameraComponent()->GetRelativeRotation();
		NewCameraRotation.Pitch = FMath::Clamp(NewCameraRotation.Pitch + DeltaPitch, -80.0f, 80.0f);

		//CurrentCamera->GetCameraComponent()->SetRelativeRotation(NewCameraRotation);
		float NewAccumulatedYaw = AccumulatedYaw + DeltaYaw;
		float NewAccumulatedPitch = AccumulatedPitch + DeltaPitch;
		NewAccumulatedYaw = FMath::Clamp(NewAccumulatedYaw, -120.0f, 120.0f);
		NewAccumulatedPitch = FMath::Clamp(NewAccumulatedPitch, -20.0f, 20.0f); 

		AccumulatedYaw = NewAccumulatedYaw;
		AccumulatedPitch = NewAccumulatedPitch;
	}
}

void AWalkerVehicles::SetTurretRotation(ACameraVehicle* CurrenCamera, FRotator TurretAngle)
{
	if (!SkeletalBaseVehicle || !AnimInstance) return;

	FName TurretName = CurrenCamera->GetAttachParentSocketName();
	AnimInstance->UpdateTurretRotation(TurretAngle, TurretName);
}

FRotator AWalkerVehicles::GetTurretAngle(ACameraVehicle* CurrenCamera, float InterpSpeed)
{
	if (!CurrentCamera || !SkeletalBaseVehicle) return CurrentAngle;
	
	CurrentAngle = FRotator(AccumulatedPitch, AccumulatedYaw, SkeletalBaseVehicle->GetSocketRotation(CurrentCamera->GetAttachParentSocketName()).Roll);
	
	return CurrentAngle;
}

