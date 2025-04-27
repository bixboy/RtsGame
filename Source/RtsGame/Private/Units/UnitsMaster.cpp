#include "Units/UnitsMaster.h"
#include "Components/BuilderComponent.h"
#include "Components/Weapons/ArtilleryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/RtsPlayerController.h"


// ------------ Setup ------------
#pragma region Setup

AUnitsMaster::AUnitsMaster(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AUnitsMaster::BeginPlay()
{
	Super::BeginPlay();

	OwnerPlayer = Cast<ARtsPlayerController>(GetInstigator()->GetController());
}

void AUnitsMaster::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AUnitsMaster, UnitTeam);
}

void AUnitsMaster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

#pragma endregion

void AUnitsMaster::SetUnitTeam(int NewTeam)
{
	if (!HasAuthority()) return;
	
	UnitTeam = NewTeam;
}

void AUnitsMaster::OnStartAttack(AActor* Target)
{
	if (!Target) return;

	if (UArtilleryComponent* ArtilleryComp = GetComponentByClass<UArtilleryComponent>())
	{
		ArtilleryComp->StartAIFiring(Target);
		return;
	}
	
	Super::OnStartAttack(Target);
}


// ------------ Selection ------------
#pragma region

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

#pragma endregion

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

int AUnitsMaster::GetTeam_Implementation()
{
	return UnitTeam;
}

UUnitsProductionDataAsset* AUnitsMaster::GetUnitData_Implementation()
{
	return UnitInfo;
}

#pragma endregion




