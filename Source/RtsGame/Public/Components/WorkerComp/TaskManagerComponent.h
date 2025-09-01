#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/WorkerData.h"
#include "Interfaces/UnitTypeInterface.h"
#include "TaskManagerComponent.generated.h"

class ARtsPlayerController;
class UWorkerComponent;


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RTSGAME_API UTaskManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTaskManagerComponent();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// === API ===
	UFUNCTION(BlueprintCallable, Category="TaskManager")
	int32 SubmitJob(const FTaskJob& JobIn);
	
	UFUNCTION(BlueprintCallable, Category="TaskManager")
	void CancelJob(int32 JobId);

	

	UFUNCTION(BlueprintCallable, Category="TaskManager")
	void RegisterIdleWorker(AActor* Worker);
	
	UFUNCTION(BlueprintCallable, Category="TaskManager")
	void UnregisterIdleWorker(AActor* Worker);

	

	UFUNCTION(BlueprintCallable, Category="TaskManager")
	void AssignJobToWorkers(const FTaskJob& Job, const TArray<AActor*> Workers);

	// Assigne un job à un seul worker
	UFUNCTION(BlueprintCallable, Category="TaskManager")
	void AssignJobToWorker(const FTaskJob& Job, AActor* Worker);


	
	UFUNCTION(BlueprintCallable, Category="TaskManager")
	AResourceDepot* FindNearestStorage(const AActor* Unit, const FResourcesCost& Needed = FResourcesCost()) const;

	AResourceDepot* FindNearestStorage(const AActor* Unit, const EResourceType NeededType) const;

private:
	UFUNCTION()
	void AssignJobs();

	UFUNCTION()
	AActor* FindBestWorkerForJob(const FTaskJob& Job) const;
	

	UPROPERTY()
	APlayerController* PC;

	UPROPERTY()
	ARtsPlayerController* RPC;


	UPROPERTY()
	bool bAutoWork = false;

	UPROPERTY()
	TArray<FTaskJob> PendingJobs;

	UPROPERTY()
	TArray<AActor*> IdleWorkers;
	
	UPROPERTY()
	int32 NextJobId = 1;

	// Gestion de fréquence d’assignation
	UPROPERTY() float AssignmentInterval = 0.2f;
	UPROPERTY() float AssignmentAccumulator = 0.f;
};
