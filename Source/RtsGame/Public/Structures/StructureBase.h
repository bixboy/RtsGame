#pragma once
#include "CoreMinimal.h"
#include "Data/DataRts.h"
#include "GameFramework/Actor.h"
#include "Interfaces/FactionsInterface.h"
#include "Interfaces/Selectable.h"
#include "StructureBase.generated.h"

class ARtsPlayerController;
class UHealthComponent;
class UStructureDataAsset;
class AGridManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStructureSelectedDelegate, bool, bIsSelected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBuildCompleteDelegate);


UCLASS()
class RTSGAME_API AStructureBase : public AActor, public ISelectable, public IFactionsInterface
{
	GENERATED_BODY()

public:
	AStructureBase();

	virtual void OnConstruction(const FTransform& Transform) override;

	ARtsPlayerController* GetOwnerController();

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY()
	ARtsPlayerController* OwnerController;

// ----------------------- Setup -----------------------
#pragma region Components
	
protected:	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Base")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Base")
	UHealthComponent* HealthComp;

#pragma endregion

	
// ----------------------- Interfaces -----------------------
#pragma region Interfaces
	
public:
	/*- Function -*/
	virtual void Select() override;
	virtual void Deselect() override;
	virtual void Highlight(const bool Highlight) override;

	virtual ESelectionType GetSelectionType_Implementation() override;

	virtual EFaction GetCurrentFaction_Implementation() override;

	UFUNCTION()
	bool GetIsSelected() const;

	UPROPERTY()
	FStructureSelectedDelegate OnSelected;

protected:
	/*- Variables -*/
	UPROPERTY()
	bool Selected;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Base")
	ESelectionType Type = ESelectionType::Structure;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Base")
	EFaction Faction = EFaction::DwarfExplorer;

#pragma endregion

	
// ----------------------- Data -----------------------
#pragma region Data
	
public:
	UFUNCTION()
	FStructure GetBuildData();

	UFUNCTION()
	void SetBuildData(FStructure NewData);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Base")
	UStructureDataAsset* StructureData;

	UPROPERTY(ReplicatedUsing = OnRep_BuildData)
	FStructure BuildData;

	UFUNCTION()
	void OnRep_BuildData();

#pragma endregion


// ----------------------- Build -----------------------
#pragma region Build
public:
	
	UFUNCTION()
	void StartBuild(ARtsPlayerController* RequestingPC);

	UFUNCTION()
	void DeliverResources(FResourcesCost DeliveredResources);

	UFUNCTION()
	void AddWorker();

	UFUNCTION()
	void RemoveWorker();

	UFUNCTION()
	bool GetNeedsResources(FResourcesCost& NeededResources) const;

	UFUNCTION()
	bool GetIsFullyResourced();
	
	UFUNCTION()
	bool GetIsBuilt() const;

	UPROPERTY()
	FBuildCompleteDelegate OnBuildComplete;
	
protected:
	UFUNCTION()
	void UpdateConstruction();

	UFUNCTION()
	void OnConstructionCompleted();
	
	/*--- Replication ---*/
	UFUNCTION(Server, Reliable)
	void Server_StartBuild(ARtsPlayerController* RequestingPC);

	UFUNCTION(Server, Reliable)
	void Server_NewWorker(int NewWorker);

	UFUNCTION(Server, Reliable)
	void Server_DeliverResources(FResourcesCost DeliveredResources);

	UFUNCTION()
	void OnRep_CurrentStepIndex();
	
	/*--- Variables ---*/
	UPROPERTY(Replicated)
	float BuildElapsedTime;

	UPROPERTY()
	FTimerHandle ConstructionTimerHandle;
	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentStepIndex)
	int32 CurrentStepIndex = 1;

	UPROPERTY(Replicated)
	FResourcesCost CurrentResources;

	UPROPERTY()
	FResourcesCost MissingResourcesForBuild;

	UPROPERTY(Replicated)
	FResourcesCost TotalResource;

	UPROPERTY(Replicated)
	int CurrentBuilder = 1;

	UPROPERTY(Replicated)
	bool bIsBuilt;

#pragma endregion	
	
};
