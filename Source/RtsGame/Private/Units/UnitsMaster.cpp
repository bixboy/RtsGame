#include "Units/UnitsMaster.h"
#include "Components/BuilderComponent.h"
#include "Components/RtsResourcesComponent.h"


// ------------ Setup ------------
#pragma region Setup

AUnitsMaster::AUnitsMaster(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AUnitsMaster::BeginPlay()
{
	Super::BeginPlay();
}

void AUnitsMaster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

#pragma endregion

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

#pragma endregion




