#include "Units/BuilderUnits.h"
#include "Components/BuilderComponent.h"
#include "Components/ResourceCollector.h"
#include "Components/RtsResourcesComponent.h"


// ====== Setup ======
#pragma region Setup

ABuilderUnits::ABuilderUnits(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<UBuilderComponent>(TEXT("CommandComponent")))
{
	PrimaryActorTick.bCanEverTick = true;

	BuilderComp = Cast<UBuilderComponent>(CommandComp);
	ResourcesComp = CreateDefaultSubobject<URtsResourcesComponent>(TEXT("ResourcesComponent"));
	ResourcesCollectorComp = CreateDefaultSubobject<UResourceCollector>(TEXT("ResourcesCollectorComponent"));
}

void ABuilderUnits::BeginPlay()
{
	Super::BeginPlay();
}

#pragma endregion


// ====== Movement ======
void ABuilderUnits::MoveToBuild_Implementation(AStructureBase* BuildDest)
{
	if (!BuildDest->GetIsBuilt() || BuildDest->GetIsInUpgrading())
	{
		BuilderComp->StartBuilding(BuildDest);
	}
}

void ABuilderUnits::MoveToResourceNode_Implementation(AResourceNode* Node)
{
	ResourcesCollectorComp->StartMoveToResource(Node);
}

void ABuilderUnits::MoveToResourceStorage_Implementation(AResourceDepot* Storage)
{
	ResourcesCollectorComp->StartMoveToStorage(Storage);
}


URtsResourcesComponent* ABuilderUnits::GetResourcesComp()
{
	return ResourcesComp;
}