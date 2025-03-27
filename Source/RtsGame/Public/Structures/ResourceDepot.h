#pragma once
#include "CoreMinimal.h"
#include "StructureBase.h"
#include "ResourceDepot.generated.h"

class URtsResourcesComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStorageUpdatedDelegate, FResourcesCost, NewResources);

UCLASS()
class RTSGAME_API AResourceDepot : public AStructureBase
{
	GENERATED_BODY()

public:
	AResourceDepot();

	UFUNCTION()
	FResourcesCost GetStorage();

	UFUNCTION()
	void AddResources(FResourcesCost NewResources);

	UFUNCTION()
	void RemoveResources(FResourcesCost NewResources);

	UPROPERTY()
	FOnStorageUpdatedDelegate OnStorageUpdated;

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnStorageUpdate(const FResourcesCost& NewResources);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	URtsResourcesComponent* ResourcesComp;
	
};
