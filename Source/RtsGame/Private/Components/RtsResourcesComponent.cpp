#include "Components/RtsResourcesComponent.h"

#include "Net/UnrealNetwork.h"


URtsResourcesComponent::URtsResourcesComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URtsResourcesComponent::BeginPlay()
{
	Super::BeginPlay();
}

void URtsResourcesComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(URtsResourcesComponent, CurrentResources);
}

// ===================== //
// ====== Private ====== //
// ===================== //

void URtsResourcesComponent::ApplyResourceChange(const FResourcesCost& NewResources)
{
	CurrentResources = NewResources.GetClamped(MaxResource);
	OnRep_CurrentResources();

	// Réplication
	Multicast_ApplyResourceChange(CurrentResources);

#if WITH_EDITOR
	UE_LOG(LogTemp, Log, TEXT("[%s] Resources updated → W:%d F:%d M:%d"),
		*GetOwner()->GetName(),
		CurrentResources.Woods,
		CurrentResources.Food,
		CurrentResources.Metal);
#endif
}

void URtsResourcesComponent::Multicast_ApplyResourceChange_Implementation(const FResourcesCost& NewResources)
{
	CurrentResources = NewResources;
	OnRep_CurrentResources();
}

// ===================== //
// ====== Public ====== //
// ===================== //

void URtsResourcesComponent::SetResources(FResourcesCost NewResources)
{
	if (!GetOwner()->HasAuthority()) return;
	ApplyResourceChange(NewResources);
}

void URtsResourcesComponent::AddResources(FResourcesCost NewResources)
{
	if (!GetOwner()->HasAuthority()) return;
	ApplyResourceChange(CurrentResources + NewResources);
}

void URtsResourcesComponent::RemoveResources(FResourcesCost ResourcesToRemove)
{
	if (!GetOwner()->HasAuthority()) return;

	// On ne retire que ce qu’on peut
	FResourcesCost Clamped = CurrentResources.GetClamped(ResourcesToRemove);
	ApplyResourceChange(CurrentResources - Clamped);
}

FResourcesCost URtsResourcesComponent::GetResources() const
{
	return CurrentResources;
}

FResourcesCost URtsResourcesComponent::GetMaxResources() const
{
	return MaxResource;
}

void URtsResourcesComponent::OnRep_CurrentResources()
{
	OnResourcesChanged.Broadcast(CurrentResources);
}

// ===================== //
// == Par Type unique == //
// ===================== //

void URtsResourcesComponent::SetResource(EResourceType Type, int32 Amount)
{
	if (!GetOwner()->HasAuthority() || Amount < 0) return;

	int32 Max = MaxResource.GetByType(Type);
	FResourcesCost New = CurrentResources;
	New.GetByType(Type) = FMath::Clamp(Amount, 0, Max);

	ApplyResourceChange(New);
}

void URtsResourcesComponent::AddResource(EResourceType Type, int32 Amount)
{
	if (!GetOwner()->HasAuthority() || Amount <= 0) return;

	FResourcesCost New = CurrentResources;
	int32& Curr = New.GetByType(Type);
	int32 Max   = MaxResource.GetByType(Type);
	Curr = FMath::Clamp(Curr + Amount, 0, Max);

	ApplyResourceChange(New);
}

void URtsResourcesComponent::RemoveResource(EResourceType Type, int32 Amount)
{
	if (!GetOwner()->HasAuthority() || Amount <= 0) return;

	FResourcesCost New = CurrentResources;
	int32& Curr = New.GetByType(Type);
	Curr = FMath::Clamp(Curr - Amount, 0, Curr);

	ApplyResourceChange(New);
}

int32 URtsResourcesComponent::GetResource(EResourceType Type)
{
	return CurrentResources.GetByType(Type);
}

int32 URtsResourcesComponent::GetMaxResource(EResourceType Type)
{
	return MaxResource.GetByType(Type);
}



bool URtsResourcesComponent::GetStorageIsFull(EResourceType Type)
{
	if (Type != EResourceType::None)
		return CurrentResources.GetByType(Type) >= MaxResource.GetByType(Type);

	return CurrentResources >= MaxResource;
}

bool URtsResourcesComponent::GetStorageIsEmpty(EResourceType Type)
{
	if (Type != EResourceType::None)
		return CurrentResources.GetByType(Type) <= 0;

	return !CurrentResources.HasAnyResource();
}