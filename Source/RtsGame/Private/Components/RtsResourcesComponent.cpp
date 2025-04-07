#include "Components/RtsResourcesComponent.h"

#include "Net/UnrealNetwork.h"


URtsResourcesComponent::URtsResourcesComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void URtsResourcesComponent::BeginPlay()
{
	Super::BeginPlay();
}

void URtsResourcesComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(URtsResourcesComponent, CurrentResources);
}

// ------- Add -------
void URtsResourcesComponent::AddResources(FResourcesCost NewResources)
{
	if (!GetOwner()->HasAuthority()) return;
	
	FResourcesCost NewTotal = CurrentResources + NewResources;
	CurrentResources = NewTotal.GetClamped(MaxResource);

	OnResourcesChanged.Broadcast(CurrentResources);

	UE_LOG(LogTemp, Warning, TEXT("%s: New Current Resource : %d"), *GetOwner()->GetName(), CurrentResources.Woods);
}

// ------- Remove -------
void URtsResourcesComponent::RemoveResources(FResourcesCost ResourcesToRemove)
{
	if (!GetOwner()->HasAuthority()) return;

	FResourcesCost ClampedResources = CurrentResources.GetClamped(ResourcesToRemove);
	CurrentResources = CurrentResources - ClampedResources;

	OnResourcesChanged.Broadcast(CurrentResources);

	UE_LOG(LogTemp, Warning, TEXT("%s: New Current Resource : %d"), *GetOwner()->GetName(), CurrentResources.Woods);
}


// ------- Getters -------
FResourcesCost URtsResourcesComponent::GetResources()
{
	return CurrentResources;
}

bool URtsResourcesComponent::GetStorageIsFull(EResourceType CheckResource)
{
	if (CheckResource == EResourceType::Wood)
	{
		return CurrentResources.Woods  == MaxResource.Woods;
	}
	
	if (CheckResource == EResourceType::Food)
	{
		return CurrentResources.Food  == MaxResource.Food;
	}
	
	if (CheckResource == EResourceType::Metal)
	{
		return CurrentResources.Metal  == MaxResource.Metal;
	}
	
	return CurrentResources == MaxResource;
}

bool URtsResourcesComponent::GetStorageIsEmpty(EResourceType CheckResource)
{
	if (CheckResource == EResourceType::Wood)
	{
		return CurrentResources.Woods <= 0;
	}
	
	if (CheckResource == EResourceType::Food)
	{
		return CurrentResources.Food <= 0;
	}
	
	if (CheckResource == EResourceType::Metal)
	{
		return CurrentResources.Metal <= 0;
	}
	
	return CurrentResources <= 0;
}

FResourcesCost URtsResourcesComponent::GetMaxResource()
{
	return MaxResource;
}

void URtsResourcesComponent::OnRep_CurrentResources()
{
	OnResourcesChanged.Broadcast(CurrentResources);
}

