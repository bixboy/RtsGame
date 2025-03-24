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
	if (GetOwnerRole() < ROLE_Authority)
	{
		Server_AddResources(NewResources);
		return;
	}
    
	CurrentResources = CurrentResources + NewResources;
}

void URtsResourcesComponent::Server_AddResources_Implementation(FResourcesCost NewResources)
{
	AddResources(NewResources);
}

// ------- Remove -------
void URtsResourcesComponent::RemoveResources(FResourcesCost ResourcesToRemove)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		Server_RemoveResources(ResourcesToRemove);
		return;
	}
    
	CurrentResources = CurrentResources - ResourcesToRemove;
}

void URtsResourcesComponent::Server_RemoveResources_Implementation(FResourcesCost ResourcesToRemove)
{
	RemoveResources(ResourcesToRemove);
}


FResourcesCost URtsResourcesComponent::GetResources()
{
	return CurrentResources;
}

void URtsResourcesComponent::OnRep_CurrentResources()
{
	if (GetOwner() && GetOwner()->GetInstigatorController() && GetOwner()->GetInstigatorController()->IsLocalPlayerController())
	{
		OnResourcesChanged.Broadcast(CurrentResources);
	}
}

