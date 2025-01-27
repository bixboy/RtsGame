#include "Vehicles/Walker/WalkerVehicles.h"

AWalkerVehicles::AWalkerVehicles()
{
	PrimaryActorTick.bCanEverTick = true;

	SkeletalBaseVehicle = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalBaseVehicle");
	SkeletalBaseVehicle->SetupAttachment(RootComponent);
}

void AWalkerVehicles::BeginPlay()
{
	Super::BeginPlay();
}

void AWalkerVehicles::WalkerMove(FVector2D Direction)
{
	FVector MovementDirection = FVector(-Direction.X, -Direction.Y, 0.0f).GetSafeNormal();
	FVector NewLocation = GetActorLocation() + MovementDirection * MovementSpeed * GetWorld()->GetDeltaSeconds();
	
	SetActorLocation(NewLocation);
	if (!SkeletalBaseVehicle->GetAnimInstance()->IsAnyMontagePlaying())
	{
		SkeletalBaseVehicle->PlayAnimation(WalkerMovementAnim, true);
	}
}

void AWalkerVehicles::WalkerStopMove()
{
	SkeletalBaseVehicle->GetAnimInstance()->StopAllMontages(0.f);
}

