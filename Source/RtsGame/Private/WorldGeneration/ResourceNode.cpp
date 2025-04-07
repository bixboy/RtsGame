#include "WorldGeneration/ResourceNode.h"
#include "Components/BoxComponent.h"
#include "Components/RtsResourcesComponent.h"
#include "Interfaces/UnitTypeInterface.h"


AResourceNode::AResourceNode()
{
	PrimaryActorTick.bCanEverTick = true;

	ResourcesComp = CreateDefaultSubobject<URtsResourcesComponent>(TEXT("ResourcesComponent"));
	ResourcesComp->OnResourcesChanged.AddDynamic(this, &AResourceNode::OnResourceUpdated);

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	RootComponent = StaticMeshComp;
	
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComp->SetupAttachment(RootComponent);
	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &AResourceNode::OnOverlapBegin);
}

void AResourceNode::BeginPlay()
{
	Super::BeginPlay();

	ResourcesComp->AddResources(ResourcesComp->GetMaxResource());
}


// -------------- Selection --------------
#pragma region Selection

void AResourceNode::Select()
{
	Selected = true;
	Highlight(Selected);
}

void AResourceNode::Deselect()
{
	Selected = false;
	Highlight(Selected);
}

void AResourceNode::Highlight(const bool Highlight)
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

#pragma endregion


FResourcesCost AResourceNode::StartResourceCollect(int TakeNumber)
{
	if (!HasAuthority() || ResourcesComp->GetStorageIsEmpty()) return FResourcesCost();

	FResourcesCost Available = ResourcesComp->GetResources();
	FResourcesCost Desired(TakeNumber);
	FResourcesCost Collected = Desired.GetClamped(Available);

	ResourcesComp->RemoveResources(Collected);
    
	UE_LOG(LogTemp, Warning, TEXT("StartResourceCollect: Collected Woods=%d, Food=%d, Metal=%d"),
		   Collected.Woods, Collected.Food, Collected.Metal);
    
	return Collected;
}

void AResourceNode::OnResourceUpdated(const FResourcesCost& NewResources)
{
	if (NewResources <= FResourcesCost())
	{
		//Destroy();
	}
}

void AResourceNode::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;
	
	if (OtherActor && OtherActor->Implements<UUnitTypeInterface>() && IUnitTypeInterface::Execute_GetUnitType(OtherActor) == EUnitsType::Builder)
	{
		float Distance = FVector::Dist(GetActorLocation(), OtherActor->GetActorLocation());
		const float CollectDistanceThreshold = 200.f;
        
		UE_LOG(LogTemp, Warning, TEXT("Overlap detected: Distance = %f"), Distance);
        
		if (Distance <= CollectDistanceThreshold)
		{
			IUnitTypeInterface::Execute_StartCollect(OtherActor);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Overlap ignored: Unit too far to collect (Distance = %f)"), Distance);
		}
	}
}

bool AResourceNode::GetIsEmpty(EResourceType CheckResource)
{
	return ResourcesComp->GetStorageIsEmpty(CheckResource);
}

EResourceType AResourceNode::GetResourceType()
{
	return ResourceType;
}

