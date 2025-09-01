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
}
void AResourceNode::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		ResourcesComp->AddResources(ResourcesComp->GetMaxResources());	
	}
}

void AResourceNode::SetupResourceNode(FResourcesCost NewResources)
{
	if (HasAuthority())
	{
		ResourcesComp->SetResources(NewResources);
	}
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


int AResourceNode::StartResourceCollect(int TakeNumber)
{
	if (!HasAuthority()) return 0;

	int32 Available = ResourcesComp->GetResource(ResourceType);
	
	if (Available <= 0) return 0;

	int32 Collected = FMath::Min(TakeNumber, Available);
	ResourcesComp->RemoveResource(ResourceType, Collected);

	return Collected;
}

void AResourceNode::OnResourceUpdated(const FResourcesCost& NewResources)
{
	if (NewResources <= FResourcesCost())
	{
		//Destroy();
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

