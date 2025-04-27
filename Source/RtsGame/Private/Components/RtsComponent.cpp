#include "Components/RtsComponent.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/UnitTypeInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Player/RtsPlayerController.h"
#include "Structures/ResourceDepot.h"
#include "Structures/StructureBase.h"
#include "Structures/UnitsProduction/UnitProduction.h"
#include "Units/BuilderUnits.h"
#include "Widgets/PlayerHudWidget.h"
#include "Widgets/SelectorWidget.h"
#include "Widgets/TopBarHudWidget.h"
#include "WorldGeneration/ResourceNode.h"


// -------------------- Setup --------------------
#pragma region Setup

URtsComponent::URtsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void URtsComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(URtsComponent, BuildToSpawn, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(URtsComponent, CurrentBuilds, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(URtsComponent, CurrentStorages, COND_OwnerOnly);
}

void URtsComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!RtsController)
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
	
	if (RtsController && RtsController->IsLocalController())
	{
		Client_CreateRtsWidget();
	}

	if (RtsController->HasAuthority() && RtsController->IsLocalController())
	{
		//Server_CreatSpawnPoint();
	}
}

void URtsComponent::Server_CreatSpawnPoint_Implementation()
{
	if (!SpawningBuildClass || !CurrentBuilds.IsEmpty()) return;

	if (!RtsController)
	{
		RtsController = Cast<ARtsPlayerController>(GetOwner());
	}
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = RtsController;

	SpawnPoint += FVector(0.f, UKismetMathLibrary::RandomFloatInRange(100.f, -3000.f), 0);
	
	if (AStructureBase* Build = GetWorld()->SpawnActor<AStructureBase>(SpawningBuildClass, SpawnPoint, FRotator::ZeroRotator, SpawnParams))
	{
		Build->SetBuildTeam(RtsController->GetPlayerTeam());
		CurrentBuilds.Add(Build);

		if (AResourceDepot* Depot = Cast<AResourceDepot>(Build))
		{
			CurrentStorages.Add(Depot);
		}
	}
}

void URtsComponent::Client_CreateRtsWidget_Implementation()
{
	if (HudWidgetClass)
	{
		if (!PlayerHudWidget)
		{
			if (UPlayerHudWidget* Widget = CreateWidget<UPlayerHudWidget>(GetWorld(), HudWidgetClass))
			{
				PlayerHudWidget = Widget;
				Widget->AddToViewport();

				TopBarWidget = PlayerHudWidget->TopBarWidget;
				
				SelectorWidget = PlayerHudWidget->SelectorWidget;
				OnSelectedUpdate.AddDynamic(this, &URtsComponent::UpdateSelectorWidget);
			}
		}
	}
}

#pragma endregion

void URtsComponent::Client_UpdateResourceValue_Implementation(FResourcesCost NewResources)
{
	if (TopBarWidget)
	{
		TopBarWidget->UpdateResources(NewResources);
	}
}


// ========= Widget Selection =========
#pragma region Widget Selection

void URtsComponent::UpdateSelectorWidget(TArray<AActor*> NewSelection)
{
	if (NewSelection.IsEmpty())
	{
		SelectorWidget->ClearSelectionWidget();
		return;
	}

	// Show Build Panel
	if (NewSelection[0]->Implements<UBuildInterface>())
	{
		SelectorWidget->ShowBuildEntries(NewSelection);
		return;
	}

	// Show Unit Panel
	if (AUnitsMaster* Unit = Cast<AUnitsMaster>(NewSelection[0]))
	{
		SelectorWidget->ShowUnitEntries(NewSelection);
	}
	else if (!Unit)
	{
		SelectorWidget->ClearSelectionWidget();
	}
}

void URtsComponent::AddUnitToProduction(UUnitsProductionDataAsset* UnitData)
{
	TArray<AStructureBase*> Builds = GetBuilds();
	if (!Builds.IsEmpty())
	{
		for (AStructureBase* Build : Builds)
		{
			if (Build->Implements<UUnitProductionInterface>() && ISelectable::Execute_GetIsSelected(Build) && Build->GetIsBuilt())
			{
				IUnitProductionInterface::Execute_AddUnitToProduction(Build, UnitData);
			}
		}
	}
}

#pragma endregion


// ========= Units Command =========
#pragma region Command

void URtsComponent::CommandSelected(FCommandData CommandData)
{
	if (GetSelectedActors().IsEmpty()) return;
	
	ABuilderUnits* Builder = Cast<ABuilderUnits>(GetSelectedActors()[0]);
	if (CommandData.Target && Builder)
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
		if (!Soldier ||
			!Soldier->Implements<USelectable>() ||
			!Soldier->Implements<UUnitTypeInterface>() ||
			!Soldier->Implements<UFactionsInterface>()) continue;

		if (IUnitTypeInterface::Execute_GetUnitType(Soldier) == EUnitsType::Builder)
		{
			IUnitTypeInterface::Execute_MoveToResource(Soldier, Node);
		}
	}
}

void URtsComponent::Server_MoveToBuildSelected_Implementation(AStructureBase* Build)
{
	if (!Build || !GetOwner()->HasAuthority()) return;

	const auto BuildFaction = IFactionsInterface::Execute_GetCurrentFaction(Build);

	for (AActor* Soldier : SelectedActors)
	{
		if (!Soldier || 
			!Soldier->Implements<USelectable>() || 
			!Soldier->Implements<UUnitTypeInterface>() || 
			!Soldier->Implements<UFactionsInterface>()) continue;

		if (IUnitTypeInterface::Execute_GetUnitType(Soldier) == EUnitsType::Builder && IFactionsInterface::Execute_GetCurrentFaction(Soldier) == BuildFaction)
		{
			IUnitTypeInterface::Execute_MoveToBuild(Soldier, Build);
		}
	}
}

#pragma endregion


// ========= Selection =========
#pragma region Selection

void URtsComponent::Server_Select_Group(const TArray<AActor*>& ActorsToSelect)
{
	if (ActorsToSelect.IsEmpty()) return;
	
	TArray<AActor*> FilteredActors;
	int CurrentTeam = RtsController->GetPlayerTeam();

	for (AActor* Actor : ActorsToSelect)
	{
		if (Actor->Implements<UFactionsInterface>())
		{
			if (IFactionsInterface::Execute_GetTeam(Actor) == CurrentTeam || IFactionsInterface::Execute_GetTeam(Actor) == -1)
			{
				FilteredActors.Add(Actor);
			}
		}
		else
		{
			FilteredActors.Add(Actor);
		}
	}
	
	Super::Server_Select_Group(FilteredActors);
}

void URtsComponent::Server_Select(AActor* ActorToSelect)
{
	
	if (ActorToSelect->Implements<UFactionsInterface>())
	{
		int CurrentTeam = RtsController->GetPlayerTeam();
		if (IFactionsInterface::Execute_GetTeam(ActorToSelect) == CurrentTeam || IFactionsInterface::Execute_GetTeam(ActorToSelect) == -1)
		{
			Super::Server_Select(ActorToSelect);
		}
		return;
	}

	
	Super::Server_Select(ActorToSelect);
}

#pragma endregion


// ========= Spawn Builds =========
#pragma region Spawn Builds

/*- -------------- Preview Build -------------- -*/
void URtsComponent::ChangeBuildClass(FStructure BuildData)
{
	if (OwnerController->HasAuthority())
	{
		BuildToSpawn = BuildData;
		OnRep_BuildClass();
	}
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
	OnRep_BuildClass();
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

void URtsComponent::SpawnBuild(FTransform BuildTransform)
{
	Server_SpawnBuild(BuildTransform.GetLocation());
}

void URtsComponent::Server_SpawnBuild_Implementation(FVector HitLocation)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = RtsController;
	
	AStructureBase* Build = GetWorld()->SpawnActor<AStructureBase>(BuildToSpawn.BuildClass, HitLocation, FRotator::ZeroRotator, SpawnParams);
	if (Build)
	{
		Build->SetBuildTeam(RtsController->GetPlayerTeam());
		Build->SetBuildData(BuildToSpawn);
		
		if (BuildToSpawn.bNeedToBuild)
		{
			Build->StartBuild();	
		}

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

template<typename T>
TArray<T*> URtsComponent::GetBuildsOf() const
{
	TArray<T*> Out;
	Out.Reserve(CurrentBuilds.Num());

	for (AStructureBase* Base : CurrentBuilds)
	{
		if (!Base) continue;
		if (T* Casted = Cast<T>(Base))
		{
			Out.Add(Casted);
		}
	}
	return Out;
}
template TArray<AUnitProduction*> URtsComponent::GetBuildsOf<AUnitProduction>() const;


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

