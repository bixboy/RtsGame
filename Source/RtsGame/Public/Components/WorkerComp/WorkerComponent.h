// ===== File: WorkerComponent.h =====
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/RtsResourcesComponent.h"
#include "Components/WorkerComp/TaskManagerComponent.h"
#include "WorkerComponent.generated.h"


class AAIController;

UENUM(BlueprintType)
enum class EWorkerState : uint8
{
	Idle,
	MovingToTarget,
	Working,
	MovingToStorage,
	Delivering
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RTSGAME_API UWorkerComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	UWorkerComponent();
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(Server, Reliable)
	void ServerRegisterManager(UTaskManagerComponent* ManagerRef);

	UFUNCTION()
	void NotifyAssignedJob(const FTaskJob& Job);

	UFUNCTION()
	void MarkJobFinished();

	UFUNCTION()
	void MoveToTarget(AActor* Target);

protected:

	UPROPERTY()
	ARtsPlayerController* RPC;

	UPROPERTY()
	AAiControllerRts* AIController;
	
	UPROPERTY()
	UTaskManagerComponent* TaskManagerRef;

	UPROPERTY()
	URtsResourcesComponent* ResourcesComp;

	UPROPERTY()
	FTaskJob CurrentJob;

	UPROPERTY()
	bool bBusy = false;

	UPROPERTY(Transient)
	bool bIsMoving = false;

	UPROPERTY(Transient)
	AActor* CurrentTarget = nullptr;

	// Helpers internes
	UFUNCTION() virtual void StartCollect(AActor* Target);
	UFUNCTION() virtual void StartBuild(AActor* Target);
	UFUNCTION() virtual void StartDeliver(AActor* Target);
	UFUNCTION() virtual void StartAttack(AActor* Target);

	UFUNCTION() virtual void OnArrivedAtTarget(AActor* Target);

	UFUNCTION() virtual void StopAll(const FTaskJob CommandData);
};
