#pragma once
#include "CoreMinimal.h"
#include "Interfaces/FactionsInterface.h"
#include "Interfaces/UnitTypeInterface.h"
#include "Units/SoldierRts.h"
#include "UnitsMaster.generated.h"

class URtsResourcesComponent;
class UBuilderComponent;


UCLASS()
class RTSGAME_API AUnitsMaster : public ASoldierRts, public IUnitTypeInterface, public IFactionsInterface
{
	GENERATED_BODY()

public:
	AUnitsMaster(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/*------ Interfaces ------*/	
	virtual EUnitsType GetUnitType_Implementation() override;

	virtual ESelectionType GetSelectionType_Implementation() override;

	virtual EFaction GetCurrentFaction_Implementation() override;
	
	virtual void Select() override;
	virtual void Deselect() override;
	virtual void Highlight(const bool Highlight) override;

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	
	/*------ Parameter ------*/	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Rts")
	ESelectionType SelectionType = ESelectionType::Unit;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Rts")
	EFaction CurrentFaction = EFaction::DwarfExplorer;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Rts")
	EUnitsType UnitsType = EUnitsType::Builder;
	
};
