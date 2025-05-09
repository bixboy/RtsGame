﻿#include "Components/ResourceCollector.h"
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
	PrimaryComponentTick.TickInterval = 0.1f;
}

void UResourceCollector::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->HasAuthority())
	{
		OwnerUnit = Cast<AUnitsMaster>(GetOwner());
		if (OwnerUnit)
		{
			OwnerResourcesComp = OwnerUnit->GetComponentByClass<URtsResourcesComponent>();
			OwnerUnit->GetAiController()->OnNewDestination.AddDynamic(this, &UResourceCollector::StopAll);
		}	
	}
}

#pragma endregion


void UResourceCollector::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!GetOwner()->HasAuthority()) return;

	// ----- Move To Storage -----
	if (DropOffBuilding && bMoveToStorage && TargetResourceNode)
	{
		if (!DropOffBuilding->GetResourcesComp()->GetStorageIsFull(TargetResourceNode->GetResourceType()) && HasReachedActor(DropOffBuilding, 50.f))
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

			if (!TargetResourceNode->GetIsEmpty(TargetResourceNode->GetResourceType()))
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
		else
		{
			DropOffBuilding = nullptr;
			bMoveToStorage = false;
			
			MoveToNearestStorage();
		}
	}
	else if (DropOffBuilding && bMoveToStorage && !TargetResourceNode)
	{
		if (HasReachedActor(DropOffBuilding, 50.f))
		{
			StartCollectResource(DropOffBuilding);
			
			DropOffBuilding = nullptr;
			bMoveToStorage = false;
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
	if (!OwnerUnit->HasAuthority() || !ResourceNode) return;
	
	TargetResourceNode = ResourceNode;

	if (OwnerResourcesComp->GetStorageIsFull(TargetResourceNode->GetResourceType()))
	{
		MoveToNearestStorage();
		return;
	}

	if (TargetResourceNode->GetIsEmpty(TargetResourceNode->GetResourceType()))
	{
		TargetResourceNode = GetNearestResourceNode();	
		
		if (!TargetResourceNode)
		{
			StopCollect(FCommandData());
			return;
		}
	}
	
	if (!OwnerResourcesComp->GetResources().HasAnyResource() && OwnerUnit->GetAiController())
	{
		if (AAiControllerRts* AiController = OwnerUnit->GetAiController())
		{
			AiController->MoveToLocation(TargetResourceNode->GetActorLocation());
			
			bIsCollecting = true;

			if (!AiController->OnNewDestination.IsBound())
				AiController->OnNewDestination.AddDynamic(this, &UResourceCollector::StopCollect);
		}
	}
}

void UResourceCollector::StartMoveToStorage(AResourceDepot* Storage)
{
	if (!GetOwner()->HasAuthority() || !Storage) return;

	ARtsPlayerController* PC = Cast<ARtsPlayerController>(OwnerUnit->OwnerPlayer);
	if (!PC || !PC->RtsComponent)
		return;

	if (OwnerResourcesComp->GetStorageIsFull(TargetResourceNode->GetResourceType()))
	{
		return;
	}

	if (!Storage->GetStorage().HasAnyResource())
	{
		return;
	}

	OwnerUnit->GetAiController()->MoveToLocation(Storage->GetActorLocation());

	TargetResourceNode = nullptr;
	DropOffBuilding = Storage;
	
	bMoveToStorage = true;
}

void UResourceCollector::MoveToNearestStorage()
{
	if (!GetOwner()->HasAuthority()) return;
    
	ARtsPlayerController* PC = Cast<ARtsPlayerController>(OwnerUnit->OwnerPlayer);
	if (!PC || !PC->RtsComponent)
		return;
	
	AResourceDepot* NearestStorage = nullptr;
	float NearestDistance = FLT_MAX;
	
	for (AStructureBase* Build : PC->RtsComponent->GetBuilds())
	{
		AResourceDepot* Storage = Cast<AResourceDepot>(Build);
		if (!Storage) continue;
		
		if (Storage->GetIsBuilt() && !Storage->GetResourcesComp()->GetStorageIsFull(TargetResourceNode->GetResourceType()))
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
	int32 MaxAmt = OwnerResourcesComp->GetMaxResource(RT);
	int32 Available = FMath::Max(0, MaxAmt - CurrentAmt);

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

void UResourceCollector::StartCollectResource(AResourceDepot* ResourceDepot)
{
	if (ResourceDepot->GetStorage().HasAnyResource())
	{
		FResourcesCost MaxReceivable = OwnerResourcesComp->GetMaxResource() - OwnerResourcesComp->GetResources();
		FResourcesCost Transferable  = ResourceDepot->GetStorage().GetClamped(MaxReceivable);

		ResourceDepot->RemoveResources(Transferable);
		OwnerResourcesComp->AddResources(Transferable);
	}
}


// ========== Stop Actions ==========
void UResourceCollector::StopCollect(const FCommandData CommandData)
{
	if (!OwnerUnit->HasAuthority()) return;

	GetWorld()->GetTimerManager().ClearTimer(CollectionTimerHandle);
	bIsCollecting = false;
}

void UResourceCollector::StopAll(const FCommandData CommandData)
{
	if (CommandData.Target == TargetResourceNode || !OwnerUnit->HasAuthority()) return;

	GetWorld()->GetTimerManager().ClearTimer(CollectionTimerHandle);
	
	bIsCollecting = false;
	TargetResourceNode = nullptr;
	
	bMoveToStorage = false;
	DropOffBuilding = nullptr;
}


// ========== Getters ==========
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

