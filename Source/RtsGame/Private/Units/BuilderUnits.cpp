#include "Units/BuilderUnits.h"
#include "Components/BuilderComponent.h"
#include "Components/ResourceCollector.h"
#include "Components/RtsResourcesComponent.h"


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

URtsResourcesComponent* ABuilderUnits::GetResourcesComp()
{
	return ResourcesComp;
}

void ABuilderUnits::MoveToBuild_Implementation(AStructureBase* BuildDest)
{
	BuilderComp->StartBuilding(BuildDest);
}

void ABuilderUnits::MoveToResource_Implementation(AResourceNode* Node)
{
	ResourcesCollectorComp->StartMoveToResource(Node);
}

void ABuilderUnits::StartCollect_Implementation()
{
	ResourcesCollectorComp->StartCollectResource();
}

void ABuilderUnits::Select()
{
	Super::Select();
}

void ABuilderUnits::Deselect()
{
	Super::Deselect();
}

