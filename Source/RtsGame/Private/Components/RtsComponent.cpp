#include "Components/RtsComponent.h"
#include "Interfaces/UnitTypeInterface.h"
#include "Net/UnrealNetwork.h"
#include "Player/RtsPlayerController.h"
#include "Structures/StructureBase.h"


// -------------------- Setup --------------------
#pragma region Setup

URtsComponent::URtsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void URtsComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(URtsComponent, BuildToSpawn, COND_OwnerOnly);
	DOREPLIFETIME(URtsComponent, CurrentBuilds);
}

void URtsComponent::BeginPlay()
{
	Super::BeginPlay();

	RtsController = Cast<ARtsPlayerController>(GetOwner());
	
	CreatSpawnPoint();
}

void URtsComponent::CreatSpawnPoint()
{
	if (!SpawningBuild) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = RtsController;
	
	AStructureBase* Build = GetWorld()->SpawnActor<AStructureBase>(SpawningBuild, SpawnPoint, FRotator::ZeroRotator, SpawnParams);
	if (Build)
	{
		CurrentBuilds.Add(Build);
	}
}

#pragma endregion


void URtsComponent::CommandSelected(FCommandData CommandData)
{
	if (CommandData.Target)
	{
		if (AStructureBase* Build = Cast<AStructureBase>(CommandData.Target))
		{
			Server_MoveToBuildSelected(Build);
			return;
		}
	}
	
	Super::CommandSelected(CommandData);
}

void URtsComponent::Server_MoveToBuildSelected_Implementation(AStructureBase* Build)
{
	if (!GetOwner()->HasAuthority() || !Build) return;
	
	for (AActor* Soldier : SelectedActors)
	{
		if (!Soldier || !Soldier->Implements<USelectable>() || !Soldier->Implements<UUnitTypeInterface>() || !Soldier->Implements<UFactionsInterface>()) continue;
		
		if (IFactionsInterface::Execute_GetCurrentFaction(Soldier) == IFactionsInterface::Execute_GetCurrentFaction(Build) && IUnitTypeInterface::Execute_GetUnitType(Soldier) == EUnitsType::Builder)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, "Go To Build: " + Build->GetName());
			
			IUnitTypeInterface::Execute_MoveToBuild(Soldier, Build);
		}
	}
}


// -------------------- Build Selection --------------------
#pragma region Build Selection

void URtsComponent::ChangeBuildClass(FStructure BuildData)
{
	if (OwnerController->HasAuthority())
		BuildToSpawn = BuildData;
	else
		Server_ChangeBuildClass(BuildData);
		
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

TArray<AStructureBase*> URtsComponent::GetBuilds()
{
	TArray<AStructureBase*> Builds;
	
	CurrentBuilds.RemoveAll([](AStructureBase* Build) { return Build == nullptr; });
	Builds.Append(CurrentBuilds);

	return Builds;
}

/*- -------------- Server Function -------------- -*/
void URtsComponent::Server_ChangeBuildClass_Implementation(FStructure BuildData)
{
	BuildToSpawn = BuildData;
}

void URtsComponent::Server_ClearPreviewClass_Implementation()
{
	UnitToSpawn = nullptr;
	BuildToSpawn = FStructure();
}

void URtsComponent::OnRep_BuildClass()
{
	OnBuildUpdated.Broadcast(BuildToSpawn);
}

void URtsComponent::Server_SpawnBuild_Implementation(FVector HitLocation)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = RtsController;
	
	AStructureBase* Build = GetWorld()->SpawnActor<AStructureBase>(BuildToSpawn.BuildClass, HitLocation, FRotator::ZeroRotator, SpawnParams);
	if (Build)
	{
		Build->SetBuildData(BuildToSpawn);
		Build->StartBuild(RtsController);

		CurrentBuilds.Add(Build);

		if (SelectedActors.Num() > 0)
		{
			FCommandData Command;
			Command.Target = Build;
			Command.Type = ECommandType::CommandMove;
			
			CommandSelected(Command);
		}
	}
}

#pragma endregion

