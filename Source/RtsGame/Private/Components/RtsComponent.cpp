#include "Components/RtsComponent.h"

#include "Net/UnrealNetwork.h"
#include "Structures/StructureBase.h"


URtsComponent::URtsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void URtsComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(URtsComponent, BuildToSpawn, COND_OwnerOnly);
}

void URtsComponent::BeginPlay()
{
	Super::BeginPlay();
}

// -------------------- Build Selection --------------------
#pragma region Build Selection

void URtsComponent::ChangeBuildClass(TSubclassOf<AStructureBase> BuildClass)
{
	if (OwnerController->HasAuthority())
		BuildToSpawn = BuildClass;
	else
		Server_ChangeBuildClass(BuildClass);
		
}

void URtsComponent::SpawnBuild()
{
	const FHitResult HitResult = GetMousePositionOnTerrain();
	if (!HitResult.bBlockingHit)
		return;
    
	Server_SpawnBuild(HitResult.Location);
}

void URtsComponent::ClearPreviewClass()
{
	Server_ClearPreviewClass();
}

/*- -------------- Server Function -------------- -*/
void URtsComponent::Server_ChangeBuildClass_Implementation(TSubclassOf<AStructureBase> BuildClass)
{
	BuildToSpawn = BuildClass;
}

void URtsComponent::Server_ClearPreviewClass_Implementation()
{
	UnitToSpawn = nullptr;
	BuildToSpawn = nullptr;
}

void URtsComponent::OnRep_BuildClass()
{
	OnBuildUpdated.Broadcast(BuildToSpawn);
}

void URtsComponent::Server_SpawnBuild_Implementation(FVector HitLocation)
{
	FActorSpawnParameters SpawnParams;
	AStructureBase* Build = GetWorld()->SpawnActor<AStructureBase>(BuildToSpawn, HitLocation, FRotator::ZeroRotator, SpawnParams);
	if (Build)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Spawned Build"));
	}
}

#pragma endregion

