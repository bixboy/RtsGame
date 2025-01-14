#include "CameraVehicle.h"
#include "Camera/CameraComponent.h"

ACameraVehicle::ACameraVehicle()
{
	PrimaryActorTick.bCanEverTick = true;
	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	RootComponent = CameraComp;
	CameraComp->bUsePawnControlRotation = true;
}

void ACameraVehicle::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACameraVehicle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

UCameraComponent* ACameraVehicle::GetCameraComponent()
{
	return CameraComp;
}

bool ACameraVehicle::GetIsUsed()
{
	return IsUsed;
}

void ACameraVehicle::SetIsUsed(bool bIsUsed)
{
	IsUsed = bIsUsed;
}

