#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/AiData.h"
#include "SlectionComponent.generated.h"

class ASoldierRts;
class UFormationDataAsset;
class UHudWidget;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSelectedUpdatedDelegate, TArray<AActor*>, NewSelection);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnitUpdatedDelegate, TSubclassOf<ASoldierRts>, NewUnitClass);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RTSMODE_API USelectionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&) const override;
	
	UFUNCTION()
	FHitResult GetMousePositionOnTerrain() const;

protected:
	UFUNCTION()
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	virtual void Server_CommandSelected(FCommandData CommandData);

	UPROPERTY()
	APlayerController* OwnerController;

// ------------------- Selection ---------------------------
#pragma region Selection
	
public:
	
	UFUNCTION()
	void Handle_Selection(AActor* ActorToSelect);
	void Handle_Selection(const TArray<AActor*> ActorsToSelect);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<AActor*> GetSelectedActors() const;

	UFUNCTION()
	virtual void CommandSelected(FCommandData CommandData);

	UPROPERTY()
	FSelectedUpdatedDelegate OnSelectedUpdate;

protected:
	
	UFUNCTION()
	bool ActorSelected(AActor* ActorToCheck) const;

	UFUNCTION()
	void OnRep_Selected() const;

	UPROPERTY(ReplicatedUsing = OnRep_Selected)
	TArray<AActor*> SelectedActors;


	/*- Server Replication -*/

	UFUNCTION(Server, Reliable)
	virtual void Server_Select(AActor* ActorToSelect);

	UFUNCTION(Server, Reliable)
	virtual void Server_Select_Group(const TArray<AActor*>& ActorsToSelect);

	UFUNCTION(Server, Reliable)
	void Server_DeSelect(AActor* ActorToDeSelect);

	UFUNCTION(Server, Reliable)
	void Server_ClearSelected();

	/*- Client Replication -*/
	
	UFUNCTION(Client, Reliable)
	void Client_Select(AActor* ActorToSelect);
	UFUNCTION(Client, Reliable)
	void Client_Deselect(AActor* ActorToDeselect);

#pragma endregion	

// ------------------- Formation ---------------------------
#pragma region Formation
public:
	UFUNCTION()
	void UpdateSpacing(const float NewSpacing);
	
	UFUNCTION()
	bool HasGroupSelection() const;

	UFUNCTION()
	void CreateHud();

	UFUNCTION()
	void UpdateFormation(EFormation Formation);

protected:

	/*- Functions -*/
	UFUNCTION()
	UFormationDataAsset* GetFormationData() const;
	
	UFUNCTION()
	void CalculateOffset(const int Index, FCommandData& CommandData);

	UFUNCTION()
	void RefreshFormation(bool bIsSpacing);


	/*- Variables -*/
	
	UPROPERTY(EditAnywhere, Category = "Settings|UI")
	TArray<UFormationDataAsset*> FormationData;
	
	UPROPERTY(EditAnywhere, Category = "Settings|UI")
	TSubclassOf<UUserWidget> HudClass;

	UPROPERTY()
	TObjectPtr<UHudWidget> Hud;

	UPROPERTY(ReplicatedUsing = OnRep_FormationSpacing)
	float FormationSpacing;
	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentFormation)
	TEnumAsByte<EFormation> CurrentFormation = EFormation::Square;

	UPROPERTY()
	const UFormationDataAsset* CurrentFormationData;

	UPROPERTY()
	FVector LastFormationLocation;
	UPROPERTY()
	AActor* LastFormationActor;

	/*- Server Replication -*/
	UFUNCTION(Server, Reliable)
	void Server_UpdateSpacing(const float NewSpacing);
	UFUNCTION(Server, Reliable)
	void Server_UpdateFormation(EFormation Formation);

	UFUNCTION()
	void OnRep_CurrentFormation();
	UFUNCTION()
	void OnRep_FormationSpacing();
	
#pragma endregion

// ------------------- Behavior ---------------------------
#pragma region Behavior
public:
	UFUNCTION()
	void UpdateBehavior(const ECombatBehavior NewBehavior);

protected:
	UFUNCTION(Server, Reliable)
	void Server_UpdateBehavior(const ECombatBehavior NewBehavior);
	
#pragma endregion

// ------------------- Spawn Units ---------------------------
#pragma region Spawn Units
	
public:
	UFUNCTION()
	void SpawnUnits();

	UFUNCTION(Server, Reliable)
	void ChangeUnitClass(TSubclassOf<ASoldierRts> UnitClass);
	
	UPROPERTY()
	FOnUnitUpdatedDelegate OnUnitUpdated;

protected:
	UFUNCTION(Server, Reliable)
	void Server_SpawnUnits(FVector HitLocation);

	UFUNCTION()
	void OnRep_UnitClass();

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_UnitClass)
	TSubclassOf<ASoldierRts> UnitToSpawn;
	
#pragma endregion	

};
