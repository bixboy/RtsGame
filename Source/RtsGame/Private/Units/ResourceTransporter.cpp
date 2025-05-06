#include "Units/ResourceTransporter.h"
#include "Components/ResourceCollector.h"
#include "Components/RtsResourcesComponent.h"

// ====== Setup ======
#pragma region Setup

AResourceTransporter::AResourceTransporter()
{
	PrimaryActorTick.bCanEverTick = true;

	ResourcesComp = CreateDefaultSubobject<URtsResourcesComponent>(TEXT("ResourcesComponent"));
	ResourcesCollectorComp = CreateDefaultSubobject<UResourceCollector>(TEXT("ResourcesCollectorComponent"));
}

void AResourceTransporter::BeginPlay()
{
	Super::BeginPlay();
}

void AResourceTransporter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AResourceTransporter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

#pragma endregion


void AResourceTransporter::MoveToResourceStorage_Implementation(AResourceDepot* Storage)
{
	ResourcesCollectorComp->StartMoveToStorage(Storage);
}

URtsResourcesComponent* AResourceTransporter::GetResourcesComp()
{
	return ResourcesComp;
}
