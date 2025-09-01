#include "Units/BuilderUnits.h"
#include "Components/WorkerComp/BuilderComponent.h"
#include "Components/RtsResourcesComponent.h"


// ====== Setup ======
#pragma region Setup

ABuilderUnits::ABuilderUnits()
{
	PrimaryActorTick.bCanEverTick = true;

	BuilderComp = CreateDefaultSubobject<UBuilderComponent>(TEXT("BuilderComponent"));
	ResourcesComp = CreateDefaultSubobject<URtsResourcesComponent>(TEXT("ResourcesComponent"));
}

void ABuilderUnits::BeginPlay()
{
	Super::BeginPlay();
}

#pragma endregion


void ABuilderUnits::StartWork_Implementation(const FTaskJob& Job)
{
	if (BuilderComp)
	{
		BuilderComp->NotifyAssignedJob(Job);	
	}
	else
	{
		BuilderComp = GetComponentByClass<UBuilderComponent>();
		BuilderComp->NotifyAssignedJob(Job);
	}
}

URtsResourcesComponent* ABuilderUnits::GetResourcesComp()
{
	return ResourcesComp;
}