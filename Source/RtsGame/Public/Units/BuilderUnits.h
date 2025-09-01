#pragma once
#include "CoreMinimal.h"
#include "UnitsMaster.h"
#include "Components/WorkerComp/TaskManagerComponent.h"
#include "BuilderUnits.generated.h"

class UResourceCollector;

UCLASS()
class RTSGAME_API ABuilderUnits : public AUnitsMaster
{
	GENERATED_BODY()

public:
	ABuilderUnits();

	UFUNCTION(BlueprintCallable)
	URtsResourcesComponent* GetResourcesComp();

	UFUNCTION()
	virtual void StartWork_Implementation(const FTaskJob& Job) override;

protected:
	virtual void BeginPlay() override;
	
	/*------ Resources ------*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	URtsResourcesComponent* ResourcesComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UBuilderComponent* BuilderComp;
};
