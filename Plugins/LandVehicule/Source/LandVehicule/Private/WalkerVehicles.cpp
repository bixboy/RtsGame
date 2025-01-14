#include "WalkerVehicles.h"

#include "CameraVehicle.h"
#include "VehiclesAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"

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

			AnimInstance->TurretAngle.Add(Cameras[i-1]->GetAttachParentSocketName(), 0.f);
			i++;	
		}
	}

	SwitchToCamera(Cameras[0]);
}

void AWalkerVehicles::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void AWalkerVehicles::SetTurretRotation(ACameraVehicle* CurrenCamera, float TurretAngle)
{
	if (!SkeletalBaseVehicle && !AnimInstance) return;

	AnimInstance->UpdateTurretRotation(TurretAngle, CurrenCamera->GetAttachParentSocketName());
}

void AWalkerVehicles::SetTurretElevation(float TurretElevation)
{
}

void AWalkerVehicles::SwitchToCamera(ACameraVehicle* NewCamera)
{
	if (!NewCamera) return;

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	PlayerController->SetViewTargetWithBlend(this, 0.5f, EViewTargetBlendFunction::VTBlend_Cubic, 0.0f, false);
	PlayerController->SetViewTargetWithBlend(NewCamera, 0.5f);
}

ACameraVehicle* AWalkerVehicles::GeCameraInArray(int Index)
{
	return Cameras[Index];
}

float AWalkerVehicles::GetTurretAngle(ACameraVehicle* CurrenCamera, float InterpSpeed)
{
	if (!CurrenCamera || !SkeletalBaseVehicle) 
		return CurrentAngle;
	
	FRotator CameraRotation = CurrenCamera->GetCameraComponent()->GetComponentRotation();

	float TargetYaw = CameraRotation.Yaw;
	CurrentAngle = FMath::FInterpTo(CurrentAngle, TargetYaw, GetWorld()->GetDeltaSeconds(), InterpSpeed);

	return CurrentAngle;
}

