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
	
	if (HasAuthority() && OwnerController && GetStorage() > 0)
	{
		OwnerController->AddResource(GetStorage());
	}
}

void AResourceDepot::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}


void AResourceDepot::BuildDestroy()
{
	Super::BuildDestroy();

	if (HasAuthority() && OwnerController)
	{
		OwnerController->RemoveResource(GetStorage());
	}
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

FResourcesCost AResourceDepot::GetResource_Implementation()
{
	return GetStorage();
}

FResourcesCost AResourceDepot::GetMaxResource_Implementation()
{
	return ResourcesComp->GetMaxResource();
}

URtsResourcesComponent* AResourceDepot::GetResourcesComp()
{
	return ResourcesComp;
}

bool AResourceDepot::GetIsEmpty(EResourceType CheckResource)
{
	return ResourcesComp->GetStorageIsEmpty(CheckResource);
}


