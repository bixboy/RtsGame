#pragma once
#include "CoreMinimal.h"
#include "Data/DataRts.h"
#include "GameFramework/Actor.h"
#include "Interfaces/BuildInterface.h"
#include "Interfaces/FactionsInterface.h"
#include "Interfaces/Selectable.h"
#include "StructureBase.generated.h"

class AResourceNode;
class ARtsPlayerController;
class UHealthComponent;
class UStructureDataAsset;
class AGridManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStructureSelectedDelegate, bool, bIsSelected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBuildCompleteDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBuildStartDelegate, float, Progress);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBuildStopDelegate);

UCLASS()
class RTSGAME_API AStructureBase : public AActor, public ISelectable, public IFactionsInterface, public IBuildInterface
{
	GENERATED_BODY()

public:
	AStructureBase();

	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION()
	ARtsPlayerController* GetOwnerController();

	UFUNCTION()
	void SetBuildTeam(int NewTeam);

	UFUNCTION()
	UStaticMeshComponent* GetMeshComponent();

protected:
	virtual void BeginPlay() override;

	virtual void Destroyed() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	virtual void BuildDestroy();

	UPROPERTY()
	ARtsPlayerController* OwnerController;

	UPROPERTY(Replicated)
	int BuildTeam = -1;

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
	virtual void Select() override;
	
	virtual void Deselect() override;
	
	virtual void Highlight(const bool Highlight) override;

	virtual bool GetIsSelected_Implementation() override;

// -----------
	virtual ESelectionType GetSelectionType_Implementation() override;

	virtual EFaction GetCurrentFaction_Implementation() override;

	virtual int GetTeam_Implementation() override;

	virtual UStructureDataAsset* GetDataAsset_Implementation() override;

// -----------
	virtual bool GetIsBuild_Implementation() override;

	virtual float GetBuildProgress_Implementation() override;

	FStructureSelectedDelegate OnSelected;

protected:
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
	UFUNCTION()
	void OnRep_BuildData();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Base")
	UStructureDataAsset* StructureData;

	UPROPERTY(ReplicatedUsing = OnRep_BuildData)
	FStructure BuildData;

#pragma endregion


// ----------------------- Build -----------------------
#pragma region Build
public:
	
	UFUNCTION()
	void StartBuild();

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

	UFUNCTION()
	bool GetIsInBuild();

	UFUNCTION()
	int GetBuilders();

	UPROPERTY()
	FBuildCompleteDelegate OnBuildComplete;

	UPROPERTY()
	FBuildStartDelegate OnBuildStart;

	UPROPERTY()
	FBuildStopDelegate OnBuildStop;
	
protected:
	UFUNCTION()
	void UpdateConstruction();

	UFUNCTION()
	void OnConstructionCompleted();
	
	/*--- Replication ---*/
	UFUNCTION(Server, Reliable)
	void Server_StartBuild();

	UFUNCTION(Server, Reliable)
	void Server_NewWorker(int NewWorker);

	UFUNCTION(Server, Reliable)
	void Server_DeliverResources(FResourcesCost DeliveredResources);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_BuildStatueDelegate(bool NewStatue);

	UFUNCTION()
	void OnRep_CurrentStepIndex();

	UFUNCTION()
	void OnRep_BuildCompleted();

	
	/*--- Variables ---*/
	UPROPERTY(EditAnywhere, category = Settings)
	TSubclassOf<AResourceNode> ResourceNodeClass;
	
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

	UPROPERTY(ReplicatedUsing = OnRep_BuildCompleted)
	bool bIsBuilt;

	UPROPERTY(Replicated)
	bool bIsInBuild;

#pragma endregion

	
// ----------------------- Build -----------------------
#pragma region Upgrading
public:	
	UFUNCTION(BlueprintCallable, Category = "Upgrade")
	void StartUpgrade(int32 UpgradeIndex);

	UFUNCTION(Server, Reliable)
	void Server_StartUpgrade(int32 UpgradeIndex);

	UFUNCTION()
	bool GetIsInUpgrading() const;

protected:
	// Upgrade update
	UFUNCTION()
	void UpdateUpgrade();
	
	UFUNCTION()
	void OnRep_CurrentUpgradeIndex();
	
	UFUNCTION()
	void OnUpgradeCompleted();
	
	// Upgrade state
	UPROPERTY(ReplicatedUsing=OnRep_CurrentUpgradeIndex, BlueprintReadOnly, Category = "Upgrade")
	int32 CurrentUpgradeIndex;

	UPROPERTY()
	float UpgradeElapsedTime;

	UPROPERTY(Replicated)
	bool bInUpgrade = false;

	UPROPERTY(Replicated)
	bool bCanUpgraded = false;

	FTimerHandle UpgradeTimerHandle;
	
#pragma endregion	
};
