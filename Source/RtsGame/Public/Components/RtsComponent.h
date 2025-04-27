#pragma once
#include "CoreMinimal.h"
#include "Components/SlectionComponent.h"
#include "Data/DataRts.h"
#include "RtsComponent.generated.h"

class UPlayerHudWidget;
class UUnitsProductionDataAsset;
class USelectorWidget;
class AResourceDepot;
class AResourceNode;
class ARtsPlayerController;
class UTopBarHudWidget;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuildUpdatedDelegate, FStructure, NewBuildData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStoragesUpdated, TArray<AResourceDepot*>, NewStorages);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnResourceUpdated,FResourcesCost, NewResource);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RTSGAME_API URtsComponent : public USelectionComponent
{
	GENERATED_BODY()

public:
	URtsComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void AddUnitToProduction(UUnitsProductionDataAsset* UnitData);

	UFUNCTION(Client, reliable)
	void Client_UpdateResourceValue(FResourcesCost NewResources);

	UFUNCTION(Server, Reliable)
	void Server_CreatSpawnPoint();

	UFUNCTION()
	TArray<AResourceNode*> GetResourceNodes();

	UPROPERTY(BlueprintAssignable, Category="Resources")
	FOnResourceUpdated OnResourceUpdated;

protected:
	virtual void BeginPlay() override;

	virtual void CommandSelected(FCommandData CommandData) override;

	UFUNCTION(Client, reliable)
	void Client_CreateRtsWidget();
	
	UFUNCTION(Server, Reliable)
	void Server_MoveToResource(AResourceNode* Node);

	UFUNCTION()
	void UpdateSelectorWidget(TArray<AActor*> NewSelection);
	
/*- Building -*/
// --------------- Functions ---------------

	virtual void Server_Select_Group(const TArray<AActor*>& ActorsToSelect) override;

	virtual void Server_Select(AActor* ActorToSelect) override;

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

	void SpawnBuild(FTransform BuildTransform);
	
	UFUNCTION()
	TArray<AStructureBase*> GetBuilds();

	template<typename T>
	TArray<T*> GetBuildsOf() const;

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
	TSubclassOf<AStructureBase> SpawningBuildClass;

	UPROPERTY()
	FVector SpawnPoint = FVector::ZeroVector;

	UPROPERTY(Replicated)
	TArray<AStructureBase*> CurrentBuilds;

	UPROPERTY(ReplicatedUsing = OnRep_Storages)
	TArray<AResourceDepot*> CurrentStorages;

	// Widgets
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Rts")
	TSubclassOf<UPlayerHudWidget> HudWidgetClass;
	
	UPROPERTY()
	TArray<AResourceNode*> AllResourceNodes;

	// Widgets Ref
	UPROPERTY()
	UPlayerHudWidget* PlayerHudWidget;

	UPROPERTY()
	UTopBarHudWidget* TopBarWidget;

	UPROPERTY()
	USelectorWidget* SelectorWidget;
	
};
