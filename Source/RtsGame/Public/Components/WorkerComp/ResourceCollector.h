#pragma once
#include "CoreMinimal.h"
#include "WorkerComponent.h"
#include "Components/ActorComponent.h"
#include "Data/AiData.h"
#include "ResourceCollector.generated.h"

class AResourceDepot;
class AResourceNode;
class URtsResourcesComponent;
class AUnitsMaster;


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RTSGAME_API UResourceCollector : public UWorkerComponent
{
	GENERATED_BODY()
	
public:
	UResourceCollector();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void StartCollect(AActor* Target) override;

	UFUNCTION()
	void StartMoveToStorage(AResourceDepot* Storage);

	virtual void OnArrivedAtTarget(AActor* Target) override;

	// Forcer la collecte depuis un dépôt
	UFUNCTION()
	void StartCollectResource(AResourceDepot* ResourceDepot);

	// Cancel / stop
	UFUNCTION(BlueprintCallable)
	void StopCollect(const FCommandData CommandData);

	virtual void StopAll(const FTaskJob CommandData) override;

	// getters
	UFUNCTION(BlueprintCallable)
	AResourceNode* GetNearestResourceNode();

	UFUNCTION(BlueprintCallable)
	bool GetIsCollecting() const;

	UFUNCTION(BlueprintCallable)
	AResourceNode* GetTargetNode() const;

protected:
	// logique interne de collecte
	void StartCollectResource();

	void MoveToNearestStorage();

protected:
	UPROPERTY(Transient)
	AResourceNode* TargetResourceNode = nullptr;

	UPROPERTY(Transient)
	AResourceDepot* DropOffBuilding = nullptr;

	UPROPERTY(Transient)
	bool bIsCollecting = false;

	UPROPERTY(Transient)
	bool bMoveToStorage = false;

	FTimerHandle CollectionTimerHandle;

	UPROPERTY(EditAnywhere, Category="Collector")
	float CollectionDelay = 1.0f;

	UPROPERTY(EditAnywhere, Category="Collector")
	int32 ResourceCollectNumber = 10;
};
