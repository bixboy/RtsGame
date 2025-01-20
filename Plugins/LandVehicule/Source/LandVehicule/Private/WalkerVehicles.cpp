#include "WalkerVehicles.h"
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
	
	AnimInstance = Cast<UVehiclesAnimInstance>(SkeletalBaseVehicle->GetAnimInstance());

	int i = 1;
	for (UActorComponent* Component : GetComponentsByTag(UCameraComponent::StaticClass(), "PlaceCamera"))
	{
		UCameraComponent* Cam = Cast<UCameraComponent>(Component);
		ACameraVehicle* SpawnedCamera = GetWorld()->SpawnActor<ACameraVehicle>(Cam->GetComponentLocation(), Cam->GetComponentRotation());
		if (SpawnedCamera)
		{
			Cameras.AddUnique(SpawnedCamera);

			FString SocketNameString = FString::Printf(TEXT("Place%d"), i);
			FName SocketAttach = FName(*SocketNameString);
		
			GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, SocketAttach.ToString());
			
			Cameras[i - 1]->AttachToComponent(SkeletalBaseVehicle, FAttachmentTransformRules::KeepWorldTransform, SocketAttach);
			Cameras[i - 1]->SetActorRotation(Cam->GetComponentRotation());

			AnimInstance->TurretAngle.Add(Cameras[i-1]->GetAttachParentSocketName(), FRotator::ZeroRotator);
			i++;	
		}
	}

	SwitchToNextCamera();
}

void AWalkerVehicles::ApplyCameraRotation(float DeltaYaw, float DeltaPitch)
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

void AWalkerVehicles::SwitchToCamera(ACameraVehicle* NewCamera)
{
	if (!NewCamera || NewCamera->GetIsUsed()) return;

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController) return;

	for (ACameraVehicle* TempCamera : Cameras)
	{
		if (TempCamera && TempCamera->GetIsUsed() && TempCamera->GetCameraController() == PlayerController)
		{
			TempCamera->SetIsUsed(false);
			TempCamera->SetController(nullptr);
		}
	}

	NewCamera->SetIsUsed(true);
	NewCamera->SetController(PlayerController);
	CurrentCamera = NewCamera;
	PlayerController->SetViewTargetWithBlend(NewCamera, 0.1f, EViewTargetBlendFunction::VTBlend_Cubic, 0.0f, false);

	UE_LOG(LogTemp, Warning, TEXT("Switched to camera: %s for player: %s"), *NewCamera->GetName(), *PlayerController->GetName());
}

void AWalkerVehicles::SwitchToNextCamera()
{
	if (Cameras.IsEmpty()) return;

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController) return;

	for (int32 i = 0; i < Cameras.Num(); i++)
	{
		ACameraVehicle* TempCamera = Cameras[i];

		if (TempCamera && TempCamera->GetIsUsed() && TempCamera->GetCameraController() == PlayerController)
		{
			TempCamera->SetIsUsed(false);
			TempCamera->SetController(nullptr);

			for (int32 j = 1; j < Cameras.Num(); j++)
			{
				int32 NextIndex = (i + j) % Cameras.Num();
				ACameraVehicle* NextCamera = Cameras[NextIndex];

				if (NextCamera && !NextCamera->GetIsUsed())
				{
					SwitchToCamera(NextCamera);
					return;
				}
			}

			TempCamera->SetIsUsed(true);
			TempCamera->SetController(PlayerController);
			return;
		}
	}

	for (ACameraVehicle* TempCamera : Cameras)
	{
		if (TempCamera && !TempCamera->GetIsUsed())
		{
			SwitchToCamera(TempCamera);
			return;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("No available camera for player: %s"), *PlayerController->GetName());
}

ACameraVehicle* AWalkerVehicles::GeCameraInArray(int Index)
{
	return Cameras[Index];
}


void AWalkerVehicles::SetTurretRotation(ACameraVehicle* CurrenCamera, FRotator TurretAngle)
{
	if (!SkeletalBaseVehicle || !AnimInstance) return;

	FName TurretName = CurrenCamera->GetAttachParentSocketName();
	AnimInstance->UpdateTurretRotation(TurretAngle, TurretName);
}

void AWalkerVehicles::SetTurretElevation(float TurretElevation)
{
}

FRotator AWalkerVehicles::GetTurretAngle(ACameraVehicle* CurrenCamera, float InterpSpeed)
{
	if (!CurrentCamera || !SkeletalBaseVehicle)
		return CurrentAngle;
	
	CurrentAngle = FRotator(AccumulatedPitch, AccumulatedYaw, SkeletalBaseVehicle->GetSocketRotation(CurrentCamera->GetAttachParentSocketName()).Roll);
	

	return CurrentAngle;
}

