#pragma once

#include "CoreMinimal.h"
#include "Data/AiData.h"
#include "Data/FormationDataAsset.h"
#include "GameFramework/PlayerController.h"
#include "PlayerControllerRts.generated.h"

class UHudWidget;
struct FCommandData;
class UInputMappingContext;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSelectedUpdatedDelegate);

UCLASS()
class RTSGAME_API APlayerControllerRts : public APlayerController
{
	GENERATED_BODY()

public:
	APlayerControllerRts(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&) const override;

	UFUNCTION()
	FVector GetMousePositionOnTerrain() const;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void Server_CommandSelected(FCommandData CommandData);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

// Selection	
#pragma region Selection
public:
	
	UFUNCTION()
	void Handle_Selection(AActor* ActorToSelect);
	void Handle_Selection(TArray<AActor*> ActorsToSelect);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<AActor*> GetSelectedActors();

	UFUNCTION()
	void CommandSelected(FCommandData CommandData);

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
	void Server_Select(AActor* ActorToSelect);

	UFUNCTION(Server, Reliable)
	void Server_Select_Group(const TArray<AActor*>& ActorsToSelect);

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

// Formation
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
	void CreateFormationData();
	UFUNCTION()
	void OnFormationDataLoaded(TArray<FPrimaryAssetId> Formations);
	
	UFUNCTION()
	UFormationDataAsset* GetFormationData() const;
	
	UFUNCTION()
	void CalculateOffset(const int Index, FCommandData& CommandData);
	UFUNCTION()
	bool IsPositionValid(const FVector& Position);

	/*- Variables -*/
	
	UPROPERTY(EditAnywhere, Category = "Settings|Formations")
	int UnitsPerRow = 5;

	UPROPERTY()
	TObjectPtr<UAssetManager> AssetManager;
	UPROPERTY()
	TArray<UFormationDataAsset*> FormationData;

	UPROPERTY()
	TObjectPtr<UHudWidget> Hud;
	UPROPERTY(EditAnywhere, Category = "Settings|UI")
	TSubclassOf<UUserWidget> HudClass;

	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentFormation)
	TEnumAsByte<EFormation> CurrentFormation;
	
	UPROPERTY(ReplicatedUsing = OnRep_FormationSpacing)
	float FormationSpacing;
	

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
	
};
