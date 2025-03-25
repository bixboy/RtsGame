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
}

void URtsComponent::BeginPlay()
{
	Super::BeginPlay();

	RtsController = Cast<ARtsPlayerController>(GetOwner());
}

#pragma endregion


void URtsComponent::Server_CommandSelected(FCommandData CommandData)
{
	if (!CommandData.Target || !Cast<AStructureBase>(CommandData.Target))
	{
		Super::Server_CommandSelected(CommandData);
	}

	for (AActor* Soldier : SelectedActors)
	{
		if (!Soldier || !Soldier->Implements<USelectable>() || !Soldier->Implements<UUnitTypeInterface>() || !Soldier->Implements<UFactionsInterface>()) continue;
		
		if (CommandData.Target && IFactionsInterface::Execute_GetCurrentFaction(Soldier) == IFactionsInterface::Execute_GetCurrentFaction(CommandData.Target) &&
			IUnitTypeInterface::Execute_GetUnitType(Soldier) == EUnitsType::Builder)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, "Go To Build: " + CommandData.Target->GetName());
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

