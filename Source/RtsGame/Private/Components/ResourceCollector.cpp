#include "Components/ResourceCollector.h"
#include "Components/RtsComponent.h"
#include "Components/RtsResourcesComponent.h"
#include "Player/RtsPlayerController.h"
#include "Structures/ResourceDepot.h"
#include "Units/UnitsMaster.h"
#include "Units/AI/AiControllerRts.h"
#include "WorldGeneration/ResourceNode.h"


// --------- Setup ---------
#pragma region Setup

UResourceCollector::UResourceCollector()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UResourceCollector::BeginPlay()
{
	Super::BeginPlay();

	OwnerUnit = Cast<AUnitsMaster>(GetOwner());
	if (OwnerUnit)
	{
		OwnerResourcesComp = OwnerUnit->GetComponentByClass<URtsResourcesComponent>();
	}
}

#pragma endregion

void UResourceCollector::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!GetOwner()->HasAuthority()) return;

	if (DropOffBuilding && bMoveToStorage)
	{
		FBox BuildBox = DropOffBuilding->GetComponentsBoundingBox();
		FVector Extent = BuildBox.GetExtent();
		float BuildRadius = FMath::Max3(Extent.X, Extent.Y, Extent.Z);
		float StopThreshold = BuildRadius + 50.f;

		float DistanceToBuild = FVector::Dist(OwnerUnit->GetActorLocation(), DropOffBuilding->GetActorLocation());

		if (DistanceToBuild <= StopThreshold)
		{
			if (OwnerUnit->GetAiController())
			{
				OwnerUnit->GetAiController()->StopMovement();
			}

			FResourcesCost CollectedResources = OwnerResourcesComp->GetResources();
			OwnerResourcesComp->RemoveResources(CollectedResources);
			DropOffBuilding->AddResources(CollectedResources);

			DropOffBuilding = nullptr;
			bMoveToStorage = false;

			if (TargetResourceNode)
			{
				StartMoveToResource(TargetResourceNode);
			}
		}
	}
}

// ------------------ Movement ------------------
void UResourceCollector::StartMoveToResource(AResourceNode* ResourceNode)
{
	if (!OwnerUnit->HasAuthority()) return;
	
	AResourceNode* TargetNode = ResourceNode;

	if (!ResourceNode || ResourceNode->GetIsEmpty())
	{
		TargetNode = GetNearestResourceNode();
		
		if (!TargetNode)
		{
			StopCollect(FCommandData());
			return;
		}
	}
	
	if (!OwnerResourcesComp->GetResources().HasAnyResource() && OwnerUnit->GetAiController())
	{
		if (AAiControllerRts* AiController = OwnerUnit->GetAiController())
		{
			TargetResourceNode = TargetNode;
			AiController->MoveToLocation(TargetNode->GetActorLocation());
			bIsCollecting = true;

			if (!AiController->OnNewDestination.IsBound())
				AiController->OnNewDestination.AddDynamic(this, &UResourceCollector::StopCollect);
		}
	}
}

void UResourceCollector::MoveToNearestStorage()
{
	if (!GetOwner()->HasAuthority())
		return;
    
	ARtsPlayerController* PC = Cast<ARtsPlayerController>(OwnerUnit->OwnerPlayer);
	if (!PC || !PC->RtsComponent)
		return;
	
	AResourceDepot* NearestStorage = nullptr;
	float NearestDistance = FLT_MAX;
	
	for (AStructureBase* Build : PC->RtsComponent->GetBuilds())
	{
		AResourceDepot* Storage = Cast<AResourceDepot>(Build);
		if (!Storage) continue;
		
		if (!Storage->GetResourcesComp()->GetStorageIsFull())
		{
			float Distance = FVector::Dist(OwnerUnit->GetActorLocation(), Storage->GetActorLocation());
			if (Distance < NearestDistance)
			{
				NearestStorage = Storage;
				NearestDistance = Distance;
			}
		}
	}

	if (NearestStorage)
	{
		OwnerUnit->GetAiController()->MoveToLocation(NearestStorage->GetActorLocation());
		
		DropOffBuilding = NearestStorage;
		bMoveToStorage = true;
	}
}

// ------------------ Resource Node ------------------
void UResourceCollector::StartCollectResource()
{
	if (!OwnerUnit->HasAuthority() || !TargetResourceNode) return;

	if (OwnerUnit->GetAiController())
		OwnerUnit->GetAiController()->StopMovement();

	if (!TargetResourceNode->GetIsEmpty() && !OwnerResourcesComp->GetStorageIsFull(TargetResourceNode->GetResourceType()))
    {
        FResourcesCost CurrentStored = OwnerResourcesComp->GetResources();
        FResourcesCost MaxStorage = OwnerResourcesComp->GetMaxResource();
        FResourcesCost AvailableCapacity = (MaxStorage - CurrentStored).GetClamped(MaxStorage);
		
        FResourcesCost Desired(ResourceCollectNumber);
        FResourcesCost ToCollect = Desired.GetClamped(AvailableCapacity);
		
        FResourcesCost CollectedCandidate = TargetResourceNode->StartResourceCollect(ResourceCollectNumber);
        FResourcesCost Collected = CollectedCandidate.GetClamped(AvailableCapacity);

        OwnerResourcesComp->AddResources(Collected);
    }
	else if (!OwnerResourcesComp->GetStorageIsFull(TargetResourceNode->GetResourceType()))
	{
		StopCollect(FCommandData());
		MoveToNearestStorage();
	}

    if (OwnerResourcesComp && !OwnerResourcesComp->GetStorageIsFull(TargetResourceNode->GetResourceType()))
    {
        GetWorld()->GetTimerManager().SetTimer(CollectionTimerHandle, this, &UResourceCollector::StartCollectResource, CollectionDelay, false);
    }
    else
    {
        StopCollect(FCommandData());
        MoveToNearestStorage();
    }
}

void UResourceCollector::StopCollect(const FCommandData CommandData)
{
	if (!OwnerUnit->HasAuthority()) return;

	GetWorld()->GetTimerManager().ClearTimer(CollectionTimerHandle);
	bIsCollecting = false;
}

AResourceNode* UResourceCollector::GetNearestResourceNode()
{
	if (!GetOwner()->HasAuthority() || !TargetResourceNode) return nullptr;

	ARtsPlayerController* PC = Cast<ARtsPlayerController>(OwnerUnit->OwnerPlayer);
	if (!PC || !PC->RtsComponent)
		return nullptr;
	
	AResourceNode* NearestNode = nullptr;
	float NearestDistance = FLT_MAX;
	
	for (AResourceNode* Node : PC->RtsComponent->GetResourceNodes())
	{
		if (Node->GetResourceType() == TargetResourceNode->GetResourceType() && !Node->GetIsEmpty())
		{
			float Distance = FVector::Dist(OwnerUnit->GetActorLocation(), Node->GetActorLocation());
			if (Distance < NearestDistance)
			{
				NearestNode = Node;
				NearestDistance = Distance;
			}
		}
	}
	
	return NearestNode;
}

bool UResourceCollector::GetIsCollecting()
{
	return bIsCollecting;
}

AResourceNode* UResourceCollector::GetTargetNode()
{
	return TargetResourceNode;
}

