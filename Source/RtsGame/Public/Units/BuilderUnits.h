#pragma once
#include "CoreMinimal.h"
#include "UnitsMaster.h"
#include "BuilderUnits.generated.h"

class UResourceCollector;

UCLASS()
class RTSGAME_API ABuilderUnits : public AUnitsMaster
{
	GENERATED_BODY()

public:
	ABuilderUnits(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable)
	URtsResourcesComponent* GetResourcesComp();

	UFUNCTION()
	virtual void MoveToBuild_Implementation(AStructureBase* BuildDest) override;

	UFUNCTION()
	virtual void MoveToResource_Implementation(AResourceNode* Node) override;

	virtual void Select() override;
	virtual void Deselect() override;

protected:
	virtual void BeginPlay() override;
	
	/*------ Resources ------*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	URtsResourcesComponent* ResourcesComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UBuilderComponent* BuilderComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UResourceCollector* ResourcesCollectorComp;
};
