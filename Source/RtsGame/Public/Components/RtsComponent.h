#pragma once
#include "CoreMinimal.h"
#include "Components/SlectionComponent.h"
#include "Data/DataRts.h"
#include "RtsComponent.generated.h"


class ARtsPlayerController;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuildUpdatedDelegate, FStructure, NewBuildData);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RTSGAME_API URtsComponent : public USelectionComponent
{
	GENERATED_BODY()

public:
	URtsComponent();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void ChangeBuildClass(FStructure BuildData);

	UFUNCTION()
	void SpawnBuild();

	UFUNCTION()
	void ClearPreviewClass();

	UFUNCTION()
	TArray<AStructureBase*> GetBuilds();

	UPROPERTY()
	FOnBuildUpdatedDelegate OnBuildUpdated;

protected:
	virtual void BeginPlay() override;

	virtual void CommandSelected(FCommandData CommandData) override;

	UFUNCTION()
	void CreatSpawnPoint();
	

// --------------- Server Function ---------------

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
	
// --------------- Variables ---------------
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_BuildClass)
	FStructure BuildToSpawn;

	UPROPERTY(Replicated)
	TArray<AStructureBase*> CurrentBuilds;

	UPROPERTY()
	ARtsPlayerController* RtsController;

	UPROPERTY()
	FVector SpawnPoint = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Rts")
	TSubclassOf<AStructureBase> SpawningBuild;
};
