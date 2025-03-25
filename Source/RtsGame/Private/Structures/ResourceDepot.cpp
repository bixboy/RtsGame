#include "Structures/ResourceDepot.h"

#include "Components/RtsResourcesComponent.h"
#include "Net/UnrealNetwork.h"


AResourceDepot::AResourceDepot()
{
	PrimaryActorTick.bCanEverTick = true;

	ResourcesComp = CreateDefaultSubobject<URtsResourcesComponent>("ResourcesComp");
	ResourcesComp->OnResourcesChanged.AddDynamic(this, &AResourceDepot::OnStorageUpdate);
}

void AResourceDepot::BeginPlay()
{
	Super::BeginPlay();
}

void AResourceDepot::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}


// Storage
void AResourceDepot::AddResources(FResourcesCost NewResources)
{
	ResourcesComp->AddResources(NewResources);
}

void AResourceDepot::RemoveResources(FResourcesCost NewResources)
{
	ResourcesComp->RemoveResources(NewResources);
}

void AResourceDepot::OnStorageUpdate(const FResourcesCost& NewResources)
{
	OnStorageUpdated.Broadcast(NewResources);
}

FResourcesCost AResourceDepot::GetStorage()
{
	return ResourcesComp->GetResources();
}


