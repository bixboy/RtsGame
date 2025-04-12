#include "Structures/ResourceDepot.h"
#include "Components/RtsResourcesComponent.h"
#include "Player/RtsPlayerController.h"


AResourceDepot::AResourceDepot()
{
	PrimaryActorTick.bCanEverTick = true;

	ResourcesComp = CreateDefaultSubobject<URtsResourcesComponent>("ResourcesComp");
	ResourcesComp->OnResourcesChanged.AddDynamic(this, &AResourceDepot::OnStorageUpdate);
}

void AResourceDepot::BeginPlay()
{
	Super::BeginPlay();

	if (OwnerController && GetStorage() > 0)
	{
		OwnerController->AddResource(GetStorage());
	}
}

void AResourceDepot::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

// Storage
void AResourceDepot::AddResources(FResourcesCost NewResources)
{
	ResourcesComp->AddResources(NewResources);
	OwnerController->AddResource(NewResources);
}

void AResourceDepot::RemoveResources(FResourcesCost NewResources)
{
	ResourcesComp->RemoveResources(NewResources);
	OwnerController->RemoveResource(NewResources);
}

void AResourceDepot::OnStorageUpdate(const FResourcesCost& NewResources)
{
	OnStorageUpdated.Broadcast(NewResources);
}

FResourcesCost AResourceDepot::GetStorage()
{
	return ResourcesComp->GetResources();
}

URtsResourcesComponent* AResourceDepot::GetResourcesComp()
{
	return ResourcesComp;
}


