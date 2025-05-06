#pragma once
#include "CoreMinimal.h"
#include "UnitsMaster.h"
#include "ResourceTransporter.generated.h"

class UResourceCollector;


UCLASS()
class RTSGAME_API AResourceTransporter : public AUnitsMaster
{
	GENERATED_BODY()

public:
	AResourceTransporter();
	
	UFUNCTION()
	virtual void MoveToResourceStorage_Implementation(AResourceDepot* Storage) override;
	
protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	

	UFUNCTION(BlueprintCallable)
	URtsResourcesComponent* GetResourcesComp();
	

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	URtsResourcesComponent* ResourcesComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UResourceCollector* ResourcesCollectorComp;
};
