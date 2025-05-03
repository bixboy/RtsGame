#include "Components/BuilderComponent.h"
#include "Components/RtsComponent.h"
#include "Components/RtsResourcesComponent.h"
#include "Player/RtsPlayerController.h"
#include "Structures/ResourceDepot.h"
#include "Structures/StructureBase.h"
#include "Units/BuilderUnits.h"
#include "Units/AI/AiControllerRts.h"


UBuilderComponent::UBuilderComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBuilderComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBuilderComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!GetOwner()->HasAuthority()) return;

	if (TargetBuild && OwnerActor && InMovement)
	{
		FBox BuildBox = TargetBuild->GetComponentsBoundingBox();
		FVector Extent = BuildBox.GetExtent();
		
		float BuildRadius = FMath::Max3(Extent.X, Extent.Y, Extent.Z);
		float StopThreshold = BuildRadius + 100.f;

		float DistanceToBuild = FVector::Dist(OwnerActor->GetActorLocation(), TargetBuild->GetActorLocation());

		if (DistanceToBuild <= StopThreshold)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Destination reached"));

			if (OwnerAIController)
			{
				OwnerAIController->StopMovement();
			}

			if (bGoingToStorage)
			{
				if (SavedConstruction)
				{
					if (AResourceDepot* Storage = Cast<AResourceDepot>(TargetBuild))
					{
						FResourcesCost NeededResources;
						FResourcesCost AvailableResources;
					
						SavedConstruction->GetNeedsResources(NeededResources);
						AvailableResources = NeededResources.GetClamped(Storage->GetStorage());
					
						Storage->RemoveResources(AvailableResources);
						ResourcesComp->AddResources(AvailableResources);

						InMovement = false;
						MoveToBuild(SavedConstruction);
						SavedConstruction = nullptr;	
					}
				}
			}
			else
			{
				InMovement = false;
				StartInteractWithBuild();
			}
		}
	}
}

void UBuilderComponent::StartBuilding(AStructureBase* Build)
{
	IsInBuild = false;
	InMovement = false;
	bGoingToStorage = false;

	TargetBuild = nullptr;
	SavedConstruction = nullptr;
	
	OwnerAIController->OnNewDestination.RemoveDynamic(this, &UBuilderComponent::StopBuild);
	
	MoveToBuild(Build);
}

AStructureBase* UBuilderComponent::GetTargetBuild()
{
	return TargetBuild;
}

// ------------ Building ------------
#pragma region Building

void UBuilderComponent::MoveToBuild(AStructureBase* Build)
{
	if (!OwnerAIController || !GetOwner()->HasAuthority() || !Build || TargetBuild == Build)
		return;
    
	if (!ResourcesComp && OwnerActor)
		ResourcesComp = OwnerActor->GetComponentByClass<URtsResourcesComponent>();
	
	if (Build->GetIsFullyResourced())
	{
		OwnerAIController->OnNewDestination.RemoveDynamic(this, &UBuilderComponent::StopBuild);
		OwnerAIController->OnNewDestination.AddDynamic(this, &UBuilderComponent::StopBuild);
        
		FCommandData Command;
		Command.Location = Build->GetActorLocation();
		Command.Type = ECommandType::CommandMove;
		
		OwnerAIController->CommandMove(Command);

		TargetBuild = Build;
		bGoingToStorage = false;
		InMovement = true;
		
		return;
	}
	
	TargetBuild = Build;
	FResourcesCost NeededResources;
	FResourcesCost AvailableResources;
	if (!CheckIfHaveResources(NeededResources, AvailableResources) && !bGoingToStorage && SavedConstruction != TargetBuild)
	{
		MoveToNearestStorage(NeededResources);
		
		return;
	}
    
	OwnerAIController->OnNewDestination.RemoveDynamic(this, &UBuilderComponent::StopBuild);
	OwnerAIController->OnNewDestination.AddDynamic(this, &UBuilderComponent::StopBuild);
    
	FCommandData Command;
	Command.Location = Build->GetActorLocation();
	Command.Type = ECommandType::CommandMove;
    
	OwnerAIController->CommandMove(Command);
	
	TargetBuild = Build;
	bGoingToStorage = false;
	InMovement = true;
}

void UBuilderComponent::StopBuild(const FCommandData CommandData)
{
	if (!GetOwner()->HasAuthority() || !TargetBuild || bGoingToStorage)
		return;
	
	TargetBuild->RemoveWorker();
	TargetBuild = nullptr;
	IsInBuild = false;
}

void UBuilderComponent::StartInteractWithBuild()
{
	if (!TargetBuild || !GetOwner()->HasAuthority())
		return;
	
	if (!ResourcesComp || (TargetBuild->GetIsBuilt() && !TargetBuild->GetIsInUpgrading()))
		return;

	if(TargetBuild->GetIsFullyResourced())
	{
		TargetBuild->AddWorker();
		IsInBuild = true;
		return;
	}
	
	FResourcesCost NeededResources;
	FResourcesCost AvailableResources;
	if (CheckIfHaveResources(NeededResources, AvailableResources))
	{
		UE_LOG(LogTemp, Warning, TEXT("Starting Construction on %s"), *TargetBuild->GetName());

		ResourcesComp->RemoveResources(AvailableResources);
		TargetBuild->DeliverResources(AvailableResources);
		TargetBuild->AddWorker();
		IsInBuild = true;
	}
	else
	{
		MoveToNearestStorage(NeededResources);
	}
}

#pragma endregion

// ------------ Storages ------------
#pragma region Storages

void UBuilderComponent::MoveToStorage(AResourceDepot* Storage)
{
	if (!OwnerAIController || !GetOwner()->HasAuthority() || !Storage)
		return;

	OwnerAIController->OnNewDestination.RemoveDynamic(this, &UBuilderComponent::StopBuild);

	SavedConstruction = TargetBuild;
	bGoingToStorage = true;

	FCommandData Command;
	Command.Location = Storage->GetActorLocation();
	Command.Type = ECommandType::CommandMove;
    
	OwnerAIController->CommandMove(Command);
	TargetBuild = Storage;
	InMovement = true;
}

void UBuilderComponent::MoveToNearestStorage(const FResourcesCost NeededResources)
{
	if (!GetOwner()->HasAuthority())
		return;
    
	ARtsPlayerController* PC = TargetBuild->GetOwnerController();
	if (!PC || !PC->RtsComponent)
		return;

	AResourceDepot* NearestStorage = nullptr;
	float NearestDistance = FLT_MAX;
	
	for (AStructureBase* Build : PC->RtsComponent->GetBuilds())
	{
		if (TargetBuild != Build && Build->GetIsBuilt())
		{
			AResourceDepot* Storage = Cast<AResourceDepot>(Build);
			if (!Storage) continue;
			
			FResourcesCost NeededFromStorage = NeededResources.GetClamped(Storage->GetStorage());
			if (NeededFromStorage.HasAnyResource())
			{
				float Distance = FVector::Dist(OwnerActor->GetActorLocation(), Storage->GetActorLocation());
				if (Distance < NearestDistance)
				{
					NearestStorage = Storage;
					NearestDistance = Distance;
				}
			}
		}
	}

	if (NearestStorage)
	{
		MoveToStorage(NearestStorage);
	}
}

bool UBuilderComponent::CheckIfHaveResources(FResourcesCost& ResourcesNeed, FResourcesCost& AvailableResources)
{
	if (!TargetBuild || !ResourcesComp) return false;
    
	FResourcesCost BuildNeeds;
	TargetBuild->GetNeedsResources(BuildNeeds);
	
	AvailableResources = BuildNeeds.GetClamped(ResourcesComp->GetResources());
	ResourcesNeed = BuildNeeds;
	
	return AvailableResources.HasAnyResource();
}

#pragma endregion