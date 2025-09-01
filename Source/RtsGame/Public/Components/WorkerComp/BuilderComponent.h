// ===== File: BuilderComponent.h =====
#pragma once

#include "CoreMinimal.h"
#include "ResourceCollector.h"
#include "Components/ActorComponent.h"
#include "Data/DataRts.h"
#include "Structures/ResourceDepot.h"
#include "WorkerComponent.h"
#include "BuilderComponent.generated.h"

struct FTaskJob;


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RTSGAME_API UBuilderComponent : public UResourceCollector
{
	GENERATED_BODY()
	
public:
	UBuilderComponent();

	virtual void StartBuild(AActor* Target) override;

	UFUNCTION(BlueprintCallable)
	AStructureBase* GetTargetBuild() const { return TargetBuild; }

protected:
	// === Worker overrides ===
	virtual void OnArrivedAtTarget(AActor* Target) override;

	virtual void StopAll(const FTaskJob CommandData) override;

private:
	// === Helpers spécifiques au build ===
	UFUNCTION()
	bool CheckIfHaveResources(FResourcesCost& OutNeeded, FResourcesCost& OutAvailable) const;

	UFUNCTION()
	void MoveToBuild(AStructureBase* Build);

	UFUNCTION()
	void MoveToNearestStorage(const FResourcesCost& NeededResources);

	UFUNCTION()
	void StartInteractWithBuild();

	// === State ===
	UPROPERTY(Transient)
	AStructureBase* TargetBuild = nullptr;

	UPROPERTY(Transient)
	AStructureBase* SavedConstruction = nullptr;

	UPROPERTY(Transient)
	bool bGoingToStorage = false;

	UPROPERTY(Transient)
	bool bIsBuilding = false;
};
