#include "Structures/StructureBase.h"
#include "Components/HealthComponent.h"
#include "Components/RtsResourcesComponent.h"
#include "Data/StructureDataAsset.h"
#include "Net/UnrealNetwork.h"
#include "Player/RtsPlayerController.h"


// ----------------------- Setup -----------------------
#pragma region Setup

AStructureBase::AStructureBase()
{
	PrimaryActorTick.bCanEverTick = true;

	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComp"));

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
}

void AStructureBase::OnConstruction(const FTransform& Transform)
{
	#if WITH_EDITOR
		if (MeshComp && StructureData && StructureData->Structure.StructureMesh)
		{
			MeshComp->SetStaticMesh(StructureData->Structure.StructureMesh);
		}
		else if (MeshComp)
		{
			MeshComp->SetStaticMesh(nullptr);
		}
	#endif
	
	Super::OnConstruction(Transform);
}

void AStructureBase::BeginPlay()
{
	Super::BeginPlay();

	OwnerController = Cast<ARtsPlayerController>(Owner);

	if (StructureData && !StructureData->Structure.bNeedToBuild)
		SetBuildData(StructureData->Structure);
}

void AStructureBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AStructureBase, BuildData);
	DOREPLIFETIME(AStructureBase, bIsBuilt);

	DOREPLIFETIME(AStructureBase, CurrentResources);
	DOREPLIFETIME(AStructureBase, CurrentBuilder);
	
	DOREPLIFETIME(AStructureBase, CurrentStepIndex);
	DOREPLIFETIME(AStructureBase, BuildElapsedTime);
}

ARtsPlayerController* AStructureBase::GetOwnerController()
{
	return OwnerController;
}

//------------ Data
void AStructureBase::SetBuildData(FStructure NewData)
{
	BuildData = NewData;

	if (!BuildData.bNeedToBuild)
	{
		MeshComp->SetStaticMesh(BuildData.StructureMesh);
		OnConstructionCompleted();
	}
}

void AStructureBase::OnRep_BuildData()
{
	if (!MeshComp) return;

	/* if (BuildData.StructureMesh)
		MeshComp->SetStaticMesh(BuildData.StructureMesh);
	else
		MeshComp->SetStaticMesh(nullptr); */
}

#pragma endregion


// ----------------------- Selection -----------------------
#pragma region Selection

void AStructureBase::Select()
{
	Selected = true;
	Highlight(Selected);

	OnSelected.Broadcast(Selected);
}

void AStructureBase::Deselect()
{
	Selected = false;
	Highlight(Selected);

	OnSelected.Broadcast(Selected);
}

void AStructureBase::Highlight(const bool Highlight)
{
	TArray<UPrimitiveComponent*> Components;
	GetComponents<UPrimitiveComponent>(Components);
	for (UPrimitiveComponent* VisualComp : Components)
	{
		if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(VisualComp))
		{
			Prim->SetRenderCustomDepth(Highlight);
		}
	}
}

ESelectionType AStructureBase::GetSelectionType_Implementation()
{
	return Type;
}

EFaction AStructureBase::GetCurrentFaction_Implementation()
{
	return Faction;
}

bool AStructureBase::GetIsSelected() const
{
	return Selected;
}

#pragma endregion


// ----------------------- Build -----------------------
#pragma region Build

void AStructureBase::StartBuild(ARtsPlayerController* RequestingPC)
{
	Server_StartBuild(RequestingPC);
}

void AStructureBase::Server_StartBuild_Implementation(ARtsPlayerController* RequestingPC)
{
	CurrentStepIndex = 0;
	CurrentBuilder = 0;
	BuildElapsedTime = 0.f;
	
	CurrentResources = FResourcesCost();
	MissingResourcesForBuild = BuildData.BuildCost;
	
	if (BuildData.BuildSteps.Num() > 0)
	{
		MeshComp->SetStaticMesh(BuildData.BuildSteps[CurrentStepIndex]);
	}
}


// --------- Worker
void AStructureBase::AddWorker()
{
	Server_NewWorker(1);
}

void AStructureBase::RemoveWorker()
{
	Server_NewWorker(-1);
}

bool AStructureBase::GetNeedsResources(FResourcesCost& NeededResources) const
{
	if (GetIsBuilt())
	{
		return false;
	}

	if (MissingResourcesForBuild != FResourcesCost())
	{
		NeededResources = MissingResourcesForBuild;
		return true;
	}
	
	return false;
}

void AStructureBase::Server_NewWorker_Implementation(int NewWorker)
{
	CurrentBuilder = CurrentBuilder + NewWorker;

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "New Worker: " + CurrentBuilder);

	if (!GetWorld()->GetTimerManager().IsTimerActive(ConstructionTimerHandle) && CurrentBuilder > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(ConstructionTimerHandle, this, &AStructureBase::UpdateConstruction, 1.0f, true);
	}
	else if (GetWorld()->GetTimerManager().IsTimerActive(ConstructionTimerHandle) && CurrentBuilder == 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(ConstructionTimerHandle);
	}
}


// --------- Deliveries
void AStructureBase::DeliverResources(FResourcesCost DeliveredResources)
{
	Server_DeliverResources(DeliveredResources);
}

void AStructureBase::Server_DeliverResources_Implementation(FResourcesCost DeliveredResources)
{
	CurrentResources += DeliveredResources;
}


// --------- Build Update
void AStructureBase::UpdateConstruction()
{
    if (CurrentResources <= FResourcesCost() || CurrentBuilder == 0)
    {
        GetWorld()->GetTimerManager().ClearTimer(ConstructionTimerHandle);
        return;
    }
	
	float DeltaTime = GetWorld()->GetDeltaSeconds() * 60.0f;
	DeltaTime = DeltaTime * CurrentBuilder;
	
	BuildElapsedTime += DeltaTime;
	float BuildProgress;
	FResourcesCost ResourcesConsumed;

	// 🔹 Update Resources
    {
    	float Scalar = DeltaTime / BuildData.TimeToBuild;
    	
    	ResourcesConsumed = BuildData.BuildCost * Scalar;
    	ResourcesConsumed = ResourcesConsumed.GetClamped(CurrentResources);

    	CurrentResources -= ResourcesConsumed;
    	MissingResourcesForBuild -=  ResourcesConsumed;
    }

	// 🔹 Update Steps Index
    {
    	BuildProgress = (BuildElapsedTime / BuildData.TimeToBuild) * 100.0f;
    	
    	int32 NewStepIndex = FMath::FloorToInt((BuildProgress / 100.0f) * BuildData.BuildSteps.Num());
    	NewStepIndex = FMath::Clamp(NewStepIndex, 0, BuildData.BuildSteps.Num() - 1);

    	if (NewStepIndex != BuildData.BuildSteps.Num() - 1 && NewStepIndex != CurrentStepIndex)
    		CurrentStepIndex = NewStepIndex;
    }
	
	// 🔹 If Construction is finished
	if (BuildElapsedTime >= BuildData.TimeToBuild)
	{
		CurrentStepIndex++;
    	
		GetWorld()->GetTimerManager().ClearTimer(ConstructionTimerHandle);
		OnConstructionCompleted();
	}

    // 🔹 Debug Screen
    if (GEngine)
    {
        FString DebugProgress = FString::Printf(TEXT("Progression: %.2f%%"), BuildProgress);
        FString DebugCurrent = FString::Printf(TEXT("Current Resources: %d"), CurrentResources.Woods);
        FString DebugConsumed = FString::Printf(TEXT("Consumed: %d"), ResourcesConsumed.Woods);
    	FString DebugElapsed = FString::Printf(TEXT("Build Time Elapsed: %.2f / %.2f sec"), BuildElapsedTime, BuildData.TimeToBuild);

        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, DebugProgress);
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, DebugCurrent);
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Blue, DebugConsumed);
    	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, DebugElapsed);
    }
}

void AStructureBase::OnRep_CurrentStepIndex()
{
	if (MeshComp && BuildData.BuildSteps.IsValidIndex(CurrentStepIndex))
	{
		MeshComp->SetStaticMesh(BuildData.BuildSteps[CurrentStepIndex]);
	}
}

void AStructureBase::OnConstructionCompleted()
{
	bIsBuilt = true;
	OnBuildComplete.Broadcast();
}

#pragma endregion


// ----------------------- Getter -----------------------
#pragma region Getter

FStructure AStructureBase::GetBuildData()
{
	if (StructureData)
		return StructureData->Structure;

	return FStructure();
}

bool AStructureBase::GetIsBuilt() const
{
	return bIsBuilt;
}

#pragma endregion

