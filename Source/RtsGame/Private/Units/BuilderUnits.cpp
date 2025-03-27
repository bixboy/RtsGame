#include "Units/BuilderUnits.h"
#include "Components/BuilderComponent.h"
#include "Components/RtsResourcesComponent.h"


ABuilderUnits::ABuilderUnits(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<UBuilderComponent>(TEXT("CommandComponent")))
{
	PrimaryActorTick.bCanEverTick = true;

	BuilderComp = Cast<UBuilderComponent>(CommandComp);
	ResourcesComp = CreateDefaultSubobject<URtsResourcesComponent>(TEXT("ResourcesComponent"));
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

