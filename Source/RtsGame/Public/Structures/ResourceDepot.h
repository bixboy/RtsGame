#pragma once
#include "CoreMinimal.h"
#include "StructureBase.h"
#include "Interfaces/StorageBuildInterface.h"
#include "ResourceDepot.generated.h"

class URtsResourcesComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStorageUpdatedDelegate, FResourcesCost, NewResources);


UCLASS()
class RTSGAME_API AResourceDepot : public AStructureBase, public IStorageBuildInterface
{
	GENERATED_BODY()

public:
	AResourceDepot();

	// ---- Interfaces
	UFUNCTION()
	virtual FResourcesCost GetResource_Implementation() override;

	UFUNCTION()
	virtual FResourcesCost GetMaxResource_Implementation() override;

	UFUNCTION()
	virtual int32 GetResourceByType_Implementation(EResourceType ResourceType = EResourceType::None) override;

	UFUNCTION()
	virtual int32 GetMaxByTypeResource_Implementation(EResourceType ResourceType = EResourceType::None) override;


	// ---- Getters
	UFUNCTION()
	FResourcesCost GetStorage();
	int32 GetStorage(EResourceType ResourceType);

	UFUNCTION()
	FResourcesCost GetResourceMax();
	int32 GetResourceMax(EResourceType ResourceType);

	UFUNCTION()
	URtsResourcesComponent* GetResourcesComp();

	UFUNCTION()
	bool GetIsEmpty(EResourceType CheckResource);

	

	UFUNCTION()
	void AddResources(FResourcesCost NewResources);

	UFUNCTION()
	void RemoveResources(FResourcesCost NewResources);

	UPROPERTY()
	FOnStorageUpdatedDelegate OnStorageUpdated;

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BuildDestroy() override;

	UFUNCTION()
	void OnStorageUpdate(const FResourcesCost& NewResources);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	URtsResourcesComponent* ResourcesComp;
	
};
