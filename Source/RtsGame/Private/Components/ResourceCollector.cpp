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

	// ----- Move To Storage -----
	if (DropOffBuilding && bMoveToStorage)
	{
		if (HasReachedActor(DropOffBuilding, 50.f))
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

			if (TargetResourceNode && !TargetResourceNode->GetIsEmpty(TargetResourceNode->GetResourceType()))
			{
				StartMoveToResource(TargetResourceNode);
			}
			else
			{
				if (AResourceNode* Node = GetNearestResourceNode())
				{
					StartMoveToResource(Node);
				}
			}
		}
	}

	// ----- Move To Node -----
	if (TargetResourceNode && !bMoveToStorage)
	{
		if (!TargetResourceNode->GetIsEmpty(TargetResourceNode->GetResourceType()))
		{
			if (HasReachedActor(TargetResourceNode, 50.f))
			{
				if (!bIsCollecting)
				{
					bIsCollecting = true;
					StartCollectResource();
				}
			}
			else
			{
				if (bIsCollecting)
				{
					bIsCollecting = false;
				}
			}	
		}
		else
		{
			if (AResourceNode* Node = GetNearestResourceNode())
			{
				StartMoveToResource(Node);
			}
			else
			{
				if (bIsCollecting)
				{
					bIsCollecting = false;
				}
			}
		}
	}
}


// ------------------ Movement ------------------
void UResourceCollector::StartMoveToResource(AResourceNode* ResourceNode)
{
	if (!OwnerUnit->HasAuthority()) return;
	
	AResourceNode* TargetNode = ResourceNode;

	if (!ResourceNode || ResourceNode->GetIsEmpty(ResourceNode->GetResourceType()))
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

bool UResourceCollector::HasReachedActor(AActor* TargetActor, float AdditionalRadius)
{
	if (!TargetActor || !OwnerUnit) return false;

	FBox TargetBox = TargetActor->GetComponentsBoundingBox();
	FVector Extent = TargetBox.GetExtent();
	float ActorRadius = FMath::Max3(Extent.X, Extent.Y, Extent.Z);

	float Threshold = ActorRadius + AdditionalRadius;

	float Distance = FVector::Dist(OwnerUnit->GetActorLocation(), TargetActor->GetActorLocation());

	return Distance <= Threshold;
}


// ------------------ Resource Node ------------------
void UResourceCollector::StartCollectResource()
{
	if (!OwnerUnit->HasAuthority() || !TargetResourceNode) return;
	
	if (auto* AI = OwnerUnit->GetAiController())
	{
		AI->StopMovement();
	}

	// 1) Si le stockage est plein → on stoppe et on dépose
	const EResourceType RT = TargetResourceNode->GetResourceType();
	if (OwnerResourcesComp->GetStorageIsFull(RT))
	{
		StopCollect(FCommandData());
		MoveToNearestStorage();
		return;
	}

	// 2) Si la node est vide → idem
	if (TargetResourceNode->GetIsEmpty(TargetResourceNode->GetResourceType()))
	{
		StopCollect(FCommandData());
		MoveToNearestStorage();
		return;
	}

	// 3) Capacité restante pour ce type
	int32 CurrentAmt = OwnerResourcesComp->GetResource(RT);
	int32 MaxAmt     = OwnerResourcesComp->GetMaxResource(RT);
	int32 Available  = FMath::Max(0, MaxAmt - CurrentAmt);

	if (Available <= 0)
	{
		StopCollect(FCommandData());
		MoveToNearestStorage();
		return;
	}
	
	int32 ToCollect = FMath::Min(ResourceCollectNumber, Available);
	
	int32 Collected = TargetResourceNode->StartResourceCollect(ToCollect);
	if (Collected <= 0 || TargetResourceNode->GetIsEmpty(TargetResourceNode->GetResourceType()))
	{
		TargetResourceNode = nullptr;
		StopCollect(FCommandData());
		MoveToNearestStorage();
		return;
	}
	
	OwnerResourcesComp->AddResource(RT, Collected);
	
	if (!OwnerResourcesComp->GetStorageIsFull(RT) && !TargetResourceNode->GetIsEmpty(TargetResourceNode->GetResourceType()))
	{
		GetWorld()->GetTimerManager().SetTimer(
			CollectionTimerHandle,
			this,
			&UResourceCollector::StartCollectResource,
			CollectionDelay,
			false
		);
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
	if (!GetOwner()->HasAuthority()) return nullptr;

	ARtsPlayerController* PC = Cast<ARtsPlayerController>(OwnerUnit->OwnerPlayer);
	if (!PC || !PC->RtsComponent)
		return nullptr;
	
	AResourceNode* NearestNode = nullptr;
	float NearestDistSq = FLT_MAX;

	const FVector MyLoc = OwnerUnit->GetActorLocation();
	const EResourceType WantedType = TargetResourceNode ? TargetResourceNode->GetResourceType() : EResourceType::None;

	for (AResourceNode* Node : PC->RtsComponent->GetResourceNodes())
	{
		if (!Node || Node->GetResourceType() != WantedType)
			continue;

		if (Node->GetIsEmpty(TargetResourceNode->GetResourceType()))
			continue;

		float DistSq = FVector::DistSquared(MyLoc, Node->GetActorLocation());
		if (DistSq < NearestDistSq)
		{
			NearestDistSq = DistSq;
			NearestNode   = Node;
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

