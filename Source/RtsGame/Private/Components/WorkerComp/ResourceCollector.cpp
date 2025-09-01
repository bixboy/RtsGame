#include "Components/WorkerComp/ResourceCollector.h"
#include "Components/RtsComponent.h"
#include "Components/RtsResourcesComponent.h"
#include "Components/WorkerComp/TaskManagerComponent.h"
#include "Player/RtsPlayerController.h"
#include "Structures/ResourceDepot.h"
#include "Units/UnitsMaster.h"
#include "WorldGeneration/ResourceNode.h"


// --------- Setup ---------

UResourceCollector::UResourceCollector()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UResourceCollector::BeginPlay()
{
	Super::BeginPlay();
	
}

void UResourceCollector::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(CollectionTimerHandle);
	}

	Super::EndPlay(EndPlayReason);
}

/* === API called by worker override === */

void UResourceCollector::StartCollect(AActor* Target)
{
	Super::StartCollect(Target);
	
	if (!GetOwner()->HasAuthority() || !Target) return;

	TargetResourceNode = Cast<AResourceNode>(Target);

	// If our inventory is full for this resource -> deposit first
	if (ResourcesComp && ResourcesComp->GetStorageIsFull(TargetResourceNode->GetResourceType()))
	{
		MoveToNearestStorage();
		return;
	}

	// If node empty -> search new
	if (TargetResourceNode->GetIsEmpty(TargetResourceNode->GetResourceType()))
	{
		TargetResourceNode = GetNearestResourceNode();
		if (!TargetResourceNode)
		{
			StopCollect(FCommandData());
			return;
		}
	}
	
	MoveToTarget(Target);
	bIsCollecting = true;
}

void UResourceCollector::StartMoveToStorage(AResourceDepot* Storage)
{
	if (!GetOwner()->HasAuthority() || !Storage) return;

	if (!ResourcesComp || ResourcesComp->GetStorageIsFull())
		return;
	
	if (!Storage->GetStorage().HasAnyResource())
		return;

	DropOffBuilding = Storage;
	bMoveToStorage = true;

	MoveToTarget(Storage);
}

void UResourceCollector::OnArrivedAtTarget(AActor* Target)
{
	Super::OnArrivedAtTarget(Target);
	
	if (!GetOwner()->HasAuthority() || !Target) return;

	// Arrived at resource node -> start collecting
	if (Target == TargetResourceNode && !bMoveToStorage)
	{
		StartCollectResource();
		return;
	}

	// Arrived at dropoff building while returning -> deposit then resume
	if (Target == DropOffBuilding && bMoveToStorage)
	{
		// deposit
		if (ResourcesComp && DropOffBuilding)
		{
			FResourcesCost CollectedResources = ResourcesComp->GetResources();
			ResourcesComp->RemoveResources(CollectedResources);
			DropOffBuilding->AddResources(CollectedResources);
		}

		bMoveToStorage = false;
		DropOffBuilding = nullptr;
		
		// after deposit, go back to node or find next
		AResourceNode* Node = (TargetResourceNode && !TargetResourceNode->GetIsEmpty(TargetResourceNode->GetResourceType())) ? TargetResourceNode : GetNearestResourceNode();
		if (Node)
		{
			TargetResourceNode = Node;
			MoveToTarget(Node);
		}
		
		return;
	}

	// Arrived at storage to collect from it (no active node)
	if (Target == DropOffBuilding && !TargetResourceNode)
	{
		StartCollectResource(DropOffBuilding);
		DropOffBuilding = nullptr;
		bMoveToStorage = false;
	}
}

/* Collect from node (internal) */
void UResourceCollector::StartCollectResource()
{
	if (!GetOwner()->HasAuthority() || !TargetResourceNode) return;
	
	EResourceType RT = TargetResourceNode->GetResourceType();

	// inventory full ?
	if (ResourcesComp && ResourcesComp->GetStorageIsFull(RT))
	{
		StopCollect(FCommandData());
		MoveToNearestStorage();
		return;
	}

	// node empty ?
	if (TargetResourceNode->GetIsEmpty(RT))
	{
		StopCollect(FCommandData());
		MoveToNearestStorage();
		return;
	}

	// compute how much we can take
	int32 CurrentAmt = ResourcesComp ? ResourcesComp->GetResource(RT) : 0;
	int32 MaxAmt = ResourcesComp ? ResourcesComp->GetMaxResource(RT) : 0;
	int32 Available = FMath::Max(0, MaxAmt - CurrentAmt);

	if (Available <= 0)
	{
		StopCollect(FCommandData());
		MoveToNearestStorage();
		return;
	}

	int32 ToCollect = FMath::Min(ResourceCollectNumber, Available);
	int32 Collected = TargetResourceNode->StartResourceCollect(ToCollect);

	if (Collected <= 0 || TargetResourceNode->GetIsEmpty(RT))
	{
		StopCollect(FCommandData());
		MoveToNearestStorage();
		return;
	}

	if (ResourcesComp)
		ResourcesComp->AddResource(RT, Collected);

	// continue collecting if not full & node not empty
	if (ResourcesComp && !ResourcesComp->GetStorageIsFull(RT) && !TargetResourceNode->GetIsEmpty(RT))
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


void UResourceCollector::StartCollectResource(AResourceDepot* ResourceDepot)
{
	if (!GetOwner()->HasAuthority() || !ResourceDepot || !ResourcesComp)
		return;

	if (ResourceDepot->GetStorage().HasAnyResource())
	{
		FResourcesCost MaxReceivable = ResourcesComp->GetMaxResources() - ResourcesComp->GetResources();
		FResourcesCost Transferable  = ResourceDepot->GetStorage().GetClamped(MaxReceivable);

		ResourceDepot->RemoveResources(Transferable);
		ResourcesComp->AddResources(Transferable);
	}
}

void UResourceCollector::StopCollect(const FCommandData CommandData)
{
	if (!GetOwner()->HasAuthority())
		return;
	
	if (GetWorld())
		GetWorld()->GetTimerManager().ClearTimer(CollectionTimerHandle);
	
	bIsCollecting = false;
}

void UResourceCollector::StopAll(const FTaskJob CommandData)
{
	if (!GetOwner()->HasAuthority()) return;

	Super::StopAll(CommandData);

	// if stop targets current node — ignore
	if (CommandData.Target == TargetResourceNode)
		return;

	if (GetWorld())
		GetWorld()->GetTimerManager().ClearTimer(CollectionTimerHandle);

	bIsCollecting = false;
	TargetResourceNode = nullptr;
	bMoveToStorage = false;
	DropOffBuilding = nullptr;
}


/* Helpers */
void UResourceCollector::MoveToNearestStorage()
{
	if (!GetOwner()->HasAuthority())
		return;

	AResourceDepot* NearestStorage = TaskManagerRef->FindNearestStorage(GetOwner(), TargetResourceNode->GetResourceType());
	if (NearestStorage)
	{
		DropOffBuilding = NearestStorage;
		bMoveToStorage = true;

		MoveToTarget(NearestStorage);
	}
}

AResourceNode* UResourceCollector::GetNearestResourceNode()
{
	if (!GetOwner()->HasAuthority()) return nullptr;
	if (!RPC || !RPC->RtsComponent) return nullptr;

	AResourceNode* NearestNode = nullptr;
	float NearestDistSq = FLT_MAX;
	const FVector MyLoc = GetOwner()->GetActorLocation();

	for (AResourceNode* Node : RPC->RtsComponent->GetResourceNodes())
	{
		if (!Node) continue;
		if (Node->GetIsEmpty(Node->GetResourceType())) continue;

		float DistSq = FVector::DistSquared(MyLoc, Node->GetActorLocation());
		if (DistSq < NearestDistSq)
		{
			NearestDistSq = DistSq;
			NearestNode   = Node;
		}
	}

	return NearestNode;
}

bool UResourceCollector::GetIsCollecting() const { return bIsCollecting; }

AResourceNode* UResourceCollector::GetTargetNode() const { return TargetResourceNode; }

