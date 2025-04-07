#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/CommandComponent.h"
#include "Data/DataRts.h"
#include "Structures/ResourceDepot.h"
#include "BuilderComponent.generated.h"

class UResourceCollector;
class URtsResourcesComponent;
class AAiControllerRts;
class AStructureBase;
class ABuilderUnits;


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RTSGAME_API UBuilderComponent : public UCommandComponent
{
	GENERATED_BODY()

public:
	UBuilderComponent();

	UFUNCTION(BlueprintCallable)
	void StartBuilding(AStructureBase* Build);

	UFUNCTION()
	void StopBuild(const FCommandData CommandData);

	UFUNCTION()
	AStructureBase* GetTargetBuild();

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void MoveToBuild(AStructureBase* Build);

	UFUNCTION(BlueprintCallable)
	void MoveToStorage(AResourceDepot* Storage);

	UFUNCTION()
	void StartInteractWithBuild();

	UFUNCTION()
	bool CheckIfHaveResources(FResourcesCost& OutResources, FResourcesCost& AvailableResources);

	UFUNCTION()
	void MoveToNearestStorage(const FResourcesCost NeededResources);

	UPROPERTY()
	AStructureBase* TargetBuild = nullptr;

	UPROPERTY()
	AStructureBase* SavedConstruction = nullptr;

	UPROPERTY()
	bool IsInBuild = false;

	UPROPERTY()
	bool InMovement = false;

	UPROPERTY()
	bool bGoingToStorage = false;

	UPROPERTY()
	URtsResourcesComponent* ResourcesComp;
	
};
