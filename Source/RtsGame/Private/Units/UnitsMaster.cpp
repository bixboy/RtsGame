#include "Units/UnitsMaster.h"
#include "Components/BuilderComponent.h"
#include "Components/RtsResourcesComponent.h"


AUnitsMaster::AUnitsMaster()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AUnitsMaster::BeginPlay()
{
	Super::BeginPlay();
}

void AUnitsMaster::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (UnitsType == EUnitsType::Builder)
	{
		BuilderComp = CreateDefaultSubobject<UBuilderComponent>("BuilderComponent");
		ResourcesComp = CreateDefaultSubobject<URtsResourcesComponent>("ResourcesComponent");
	}
	else
	{
		if (BuilderComp)
		{
			BuilderComp->DestroyComponent();
			BuilderComp = nullptr;	
		}

		if (ResourcesComp)
		{
			ResourcesComp->DestroyComponent();
			ResourcesComp = nullptr;
		}
	}
}

void AUnitsMaster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AUnitsMaster::Select()
{
	Super::Select();
}

void AUnitsMaster::Deselect()
{
	Super::Deselect();
}

void AUnitsMaster::Highlight(const bool Highlight)
{
	Super::Highlight(Highlight);
}

// ------------ Getters ------------
#pragma region Getters

EUnitsType AUnitsMaster::GetUnitType_Implementation()
{
	return UnitsType;
}

ESelectionType AUnitsMaster::GetSelectionType_Implementation()
{
	return SelectionType;
}

EFaction AUnitsMaster::GetCurrentFaction_Implementation()
{
	return CurrentFaction;
}

URtsResourcesComponent* AUnitsMaster::GetResourcesComp()
{
	return ResourcesComp;
}

#pragma endregion




