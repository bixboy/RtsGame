#pragma once
#include "CoreMinimal.h"
#include "Components/SlectionComponent.h"
#include "RtsComponent.generated.h"

class AStructureBase;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuildUpdatedDelegate, TSubclassOf<AStructureBase>, NewBuildClass);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RTSGAME_API URtsComponent : public USelectionComponent
{
	GENERATED_BODY()

public:
	URtsComponent();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void ChangeBuildClass(TSubclassOf<AStructureBase> BuildClass);

	UFUNCTION()
	void SpawnBuild();

	UFUNCTION()
	void ClearPreviewClass();

	UPROPERTY()
	FOnBuildUpdatedDelegate OnBuildUpdated;

protected:
	virtual void BeginPlay() override;

// --------------- Server Function ---------------
	UFUNCTION(Server, Reliable)
	void Server_ChangeBuildClass(TSubclassOf<AStructureBase> BuildClass);

	UFUNCTION(Server, Reliable)
	void Server_ClearPreviewClass();

	UFUNCTION(Server, Reliable)
	void Server_SpawnBuild(FVector HitLocation);

	UFUNCTION()
	void OnRep_BuildClass();
	
// --------------- Variables ---------------
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_BuildClass)
	TSubclassOf<AStructureBase> BuildToSpawn;
};
