#pragma once
#include "CoreMinimal.h"
#include "Data/UnitsProductionDataAsset.h"
#include "Interfaces/FactionsInterface.h"
#include "Interfaces/UnitTypeInterface.h"
#include "Units/SoldierRts.h"
#include "UnitsMaster.generated.h"

class UUnitsProductionDataAsset;
class ARtsPlayerController;
class URtsResourcesComponent;
class UBuilderComponent;


UCLASS()
class RTSGAME_API AUnitsMaster : public ASoldierRts, public IUnitTypeInterface, public IFactionsInterface
{
	GENERATED_BODY()

public:
	AUnitsMaster();

	UFUNCTION()
	void SetUnitTeam(int NewTeam);

	/*------ Interfaces ------*/	
	virtual EUnitsType GetUnitType_Implementation() override;

	virtual ESelectionType GetSelectionType_Implementation() override;

	virtual EFaction GetCurrentFaction_Implementation() override;

	virtual int GetTeam_Implementation() override;

	virtual UUnitsProductionDataAsset* GetUnitData_Implementation() override;
	
	virtual void Select() override;
	virtual void Deselect() override;
	virtual void Highlight(const bool Highlight) override;

	UPROPERTY()
	ARtsPlayerController* OwnerPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Rts")
	UUnitsProductionDataAsset* UnitInfo;

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void OnStartAttack(AActor* Target) override;
	
	/*------ Parameter ------*/	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Rts")
	ESelectionType SelectionType = ESelectionType::Unit;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Rts")
	EFaction CurrentFaction = EFaction::DwarfExplorer;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Rts")
	EUnitsType UnitsType = EUnitsType::Builder;

	UPROPERTY(Replicated)
	int UnitTeam = -1;
	
};
