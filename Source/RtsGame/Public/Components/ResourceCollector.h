#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/AiData.h"
#include "ResourceCollector.generated.h"

class AResourceDepot;
class AResourceNode;
class URtsResourcesComponent;
class AUnitsMaster;


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RTSGAME_API UResourceCollector : public UActorComponent
{
	GENERATED_BODY()

public:
	UResourceCollector();

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
public:
	/*----- Functions -----*/
	UFUNCTION()
	void StartMoveToResource(AResourceNode* ResourceNode);

	UFUNCTION()
	void StartCollectResource();

	UFUNCTION()
	void StopCollect(const FCommandData CommandData);

	UFUNCTION()
	bool GetIsCollecting();

	UFUNCTION()
	AResourceNode* GetTargetNode();
	
protected:
	
	UFUNCTION()
	void MoveToNearestStorage();

	UFUNCTION()
	AResourceNode* GetNearestResourceNode();

	/*----- Settings Variables -----*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Settings)
	int ResourceCollectNumber = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Settings)
	float CollectionDelay = 1.5f;

	FTimerHandle CollectionTimerHandle;

	UPROPERTY()
	bool bMoveToStorage = false;

	UPROPERTY()
	bool bIsCollecting = false;

	/*----- Ref Variables -----*/
	UPROPERTY()
	AUnitsMaster* OwnerUnit;

	UPROPERTY()
	URtsResourcesComponent* OwnerResourcesComp;

	UPROPERTY()
	AResourceNode* TargetResourceNode;

	UPROPERTY()
	AResourceDepot* DropOffBuilding;
};
