#include "Components/RtsComponent.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/UnitTypeInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/RtsPlayerController.h"
#include "Structures/ResourceDepot.h"
#include "Structures/StructureBase.h"
#include "Widgets/PlayerResourceWidget.h"
#include "WorldGeneration/ResourceNode.h"


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
	DOREPLIFETIME_CONDITION(URtsComponent, CurrentBuilds, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(URtsComponent, CurrentStorages, COND_OwnerOnly);
}

void URtsComponent::BeginPlay()
{
	Super::BeginPlay();

	RtsController = Cast<ARtsPlayerController>(GetOwner());

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AResourceNode::StaticClass(), FoundActors);
	for (AActor* Actor : FoundActors)
	{
		if (AResourceNode* Node = Cast<AResourceNode>(Actor))
		{
			AllResourceNodes.Add(Node);
		}
	}

	if (!GetOwner()->HasAuthority())
	{
		CreateRtsWidget();	
	}

	if (GetOwner()->HasAuthority())
	{
		Server_CreatSpawnPoint();
	}
}

void URtsComponent::Server_CreatSpawnPoint_Implementation()
{
	if (!SpawningBuild) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = RtsController;
	
	AStructureBase* Build = GetWorld()->SpawnActor<AStructureBase>(SpawningBuild, SpawnPoint, FRotator::ZeroRotator, SpawnParams);
	if (Build)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawned build: %s"), *Build->GetName());
		CurrentBuilds.Add(Build);

		if (AResourceDepot* Depot = Cast<AResourceDepot>(Build))
		{
			CurrentStorages.Add(Depot);
		}
	}
}

void URtsComponent::CreateRtsWidget()
{
	if (ResourceWidgetClass)
	{
		if (!ResourceWidget)
		{
			if (UPlayerResourceWidget* Widget = CreateWidget<UPlayerResourceWidget>(GetWorld(), ResourceWidgetClass))
			{
				Widget->AddToViewport();
				ResourceWidget = Widget;
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Widget instance already exists: %p"), ResourceWidget);
		}
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
		
		if (AResourceNode* Node = Cast<AResourceNode>(CommandData.Target))
		{
			Server_MoveToResource(Node);
			return;
		}
	}
	
	Super::CommandSelected(CommandData);
}

void URtsComponent::Server_MoveToResource_Implementation(AResourceNode* Node)
{
	if (!GetOwner()->HasAuthority() || !Node) return;

	for (AActor* Soldier : SelectedActors)
	{
		if (!Soldier || !Soldier->Implements<USelectable>() || !Soldier->Implements<UUnitTypeInterface>() || !Soldier->Implements<UFactionsInterface>()) continue;

		if (IUnitTypeInterface::Execute_GetUnitType(Soldier) == EUnitsType::Builder)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, "Go To Resource: " + Node->GetName());
			
			IUnitTypeInterface::Execute_MoveToResource(Soldier, Node);
		}
	}
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

void URtsComponent::Client_UpdateResourceValue_Implementation(FResourcesCost NewResources)
{
	if (ResourceWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("Update Resource Value %d, %d, %d"), NewResources.Woods, NewResources.Food, NewResources.Metal);
		ResourceWidget->UpdateResourceValue(NewResources);
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

void URtsComponent::ClearPreviewClass()
{
	Server_ClearPreviewClass();
}

/*- -------------- Spawn Build -------------- -*/
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

void URtsComponent::SpawnBuild()
{
	const FHitResult HitResult = GetMousePositionOnTerrain();
	if (!HitResult.bBlockingHit)
		return;
    
	Server_SpawnBuild(HitResult.Location);
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
		
		if (AResourceDepot* Storage = Cast<AResourceDepot>(Build))
			CurrentStorages.Add(Storage);
			
		if (SelectedActors.Num() > 0)
		{
			FCommandData Command;
			Command.Target = Build;
			Command.Type = ECommandType::CommandMove;
			
			CommandSelected(Command);
		}
	}
}

/*- -------------- Getter -------------- -*/
TArray<AResourceNode*> URtsComponent::GetResourceNodes()
{
	return AllResourceNodes;
}

TArray<AStructureBase*> URtsComponent::GetBuilds()
{
	TArray<AStructureBase*> Builds;
	
	CurrentBuilds.RemoveAll([](AStructureBase* Build) { return Build == nullptr; });
	Builds.Append(CurrentBuilds);

	return Builds;
}

TArray<AResourceDepot*> URtsComponent::GetDepots()
{
	CurrentStorages.RemoveAll([](AResourceDepot* Storage) { return Storage == nullptr; });
	
	TArray<AResourceDepot*> Storages;
	Storages.Append(CurrentStorages);
	
	return Storages;
}

void URtsComponent::OnRep_Storages()
{
	UE_LOG(LogTemp, Warning, TEXT("GetDepots: CurrentStorages contains %d items"), CurrentStorages.Num());
	OnStoragesUpdated.Broadcast(CurrentStorages);
}


#pragma endregion

