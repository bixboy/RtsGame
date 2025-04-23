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
	
	CurrentResources = (CurrentResources + NewResources).GetClamped(MaxResource);

	Multicast_UpdateResources(CurrentResources);
	OnResourcesChanged.Broadcast(CurrentResources);

	UE_LOG(LogTemp, Warning, TEXT("%s: New Current Resource : %d"), *GetOwner()->GetName(), CurrentResources.Woods);
}

// ------- Remove -------
void URtsResourcesComponent::RemoveResources(FResourcesCost ResourcesToRemove)
{
	if (!GetOwner()->HasAuthority()) return;

	FResourcesCost Clamped = CurrentResources.GetClamped(ResourcesToRemove);
	CurrentResources = CurrentResources - Clamped;

	Multicast_UpdateResources(CurrentResources);
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

void URtsResourcesComponent::Multicast_UpdateResources_Implementation(const FResourcesCost NewResources)
{
	CurrentResources = NewResources;
	OnResourcesChanged.Broadcast(CurrentResources);
}


// -----
void URtsResourcesComponent::AddResource(EResourceType Type, int32 Amount)
{
	if (!GetOwner()->HasAuthority() || Amount <= 0) return;

	int32& Curr = CurrentResources.GetByType(Type);
	int32  Max  = MaxResource.GetByType(Type);
	Curr = FMath::Clamp(Curr + Amount, 0, Max);

	Multicast_UpdateResources(CurrentResources);
	OnResourcesChanged.Broadcast(CurrentResources);

	UE_LOG(LogTemp, Warning, TEXT("%s: +%d %s → now %d"),
		*GetOwner()->GetName(),
		Amount,
		*UEnum::GetValueAsString(Type),
		Curr);
}

void URtsResourcesComponent::RemoveResource(EResourceType Type, int32 Amount)
{
	if (!GetOwner()->HasAuthority() || Amount <= 0) return;
	
	int32& Curr = CurrentResources.GetByType(Type);
	Curr = FMath::Clamp(Curr - Amount, 0, Curr);

	Multicast_UpdateResources(CurrentResources);
	OnResourcesChanged.Broadcast(CurrentResources);

	UE_LOG(LogTemp, Warning, TEXT("%s: -%d %s → now %d"),
		*GetOwner()->GetName(),
		Amount,
		*UEnum::GetValueAsString(Type),
		Curr);
}

int32 URtsResourcesComponent::GetResource(EResourceType Type)
{
	return CurrentResources.GetByType(Type);
}

int32 URtsResourcesComponent::GetMaxResource(EResourceType Type)
{
	return MaxResource.GetByType(Type);
}

