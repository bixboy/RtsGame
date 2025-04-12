#pragma once
#include "CoreMinimal.h"
#include "Components/SlectionComponent.h"
#include "Data/DataRts.h"
#include "RtsComponent.generated.h"

class USelectorWidget;
class UPlayerResourceWidget;
class AResourceDepot;
class AResourceNode;
class ARtsPlayerController;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuildUpdatedDelegate, FStructure, NewBuildData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStoragesUpdated, TArray<AResourceDepot*>, NewStorages);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnResourceUpdated,FResourcesCost, NewResource);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RTSGAME_API URtsComponent : public USelectionComponent
{
	GENERATED_BODY()

public:
	URtsComponent();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void CreateRtsWidget();

	UFUNCTION()
	void AddUnitToProduction(UUnitsProductionDataAsset* UnitData);

	UFUNCTION(Client, reliable)
	void Client_UpdateResourceValue(FResourcesCost NewResources);

	UFUNCTION()
	TArray<AResourceNode*> GetResourceNodes();

	UPROPERTY(BlueprintAssignable, Category="Resources")
	FOnResourceUpdated OnResourceUpdated;

protected:
	virtual void BeginPlay() override;

	virtual void CommandSelected(FCommandData CommandData) override;
	
	UFUNCTION(Server, Reliable)
	void Server_CreatSpawnPoint();

	UFUNCTION(Server, Reliable)
	void Server_MoveToResource(AResourceNode* Node);

	UFUNCTION()
	void UpdateSelectorWidget(TArray<AActor*> NewSelection);
	
/*- Building -*/
// --------------- Functions ---------------

	UFUNCTION(Server, Reliable)
	void Server_MoveToBuildSelected(AStructureBase* Build);
	
	UFUNCTION(Server, Reliable)
	void Server_ChangeBuildClass(FStructure BuildData);

	UFUNCTION(Server, Reliable)
	void Server_ClearPreviewClass();

	UFUNCTION(Server, Reliable)
	void Server_SpawnBuild(FVector HitLocation);

	UFUNCTION()
	void OnRep_BuildClass();

	UFUNCTION()
	void OnRep_Storages();
	
public:
	UFUNCTION()
	void ChangeBuildClass(FStructure BuildData);

	UFUNCTION()
	void ClearPreviewClass();

	UFUNCTION()
	void SpawnBuild();
	
	UFUNCTION()
	TArray<AStructureBase*> GetBuilds();

	UFUNCTION()
	TArray<AResourceDepot*> GetDepots();

	UPROPERTY()
	FOnBuildUpdatedDelegate OnBuildUpdated;

	UPROPERTY(BlueprintAssignable, Category="Resources")
	FOnStoragesUpdated OnStoragesUpdated;
	
// --------------- Variables ---------------
protected:
	UPROPERTY()
	ARtsPlayerController* RtsController;

	// Builds
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_BuildClass)
	FStructure BuildToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Rts")
	TSubclassOf<AStructureBase> SpawningBuild;

	UPROPERTY()
	FVector SpawnPoint = FVector::ZeroVector;

	UPROPERTY(Replicated)
	TArray<AStructureBase*> CurrentBuilds;

	UPROPERTY(ReplicatedUsing = OnRep_Storages)
	TArray<AResourceDepot*> CurrentStorages;

	// Widgets
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Rts")
	TSubclassOf<UPlayerResourceWidget> ResourceWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Rts")
	TSubclassOf<USelectorWidget> SelectorWidgetClass;
	
	UPROPERTY()
	TArray<AResourceNode*> AllResourceNodes;

	UPROPERTY()
	UPlayerResourceWidget* ResourceWidget;

	UPROPERTY()
	USelectorWidget* SelectorWidget;
	
};
