#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/DataRts.h"
#include "RtsResourcesComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnResourcesChanged, const FResourcesCost&, NewResources);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RTSGAME_API URtsResourcesComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URtsResourcesComponent();

	// ===== Multi Resources =====

	UFUNCTION(BlueprintCallable, Category="Resources")
	void SetResources(FResourcesCost NewResources);
	
	UFUNCTION(BlueprintCallable, Category="Resources")
	void AddResources(FResourcesCost NewResources);

	UFUNCTION(BlueprintCallable, Category="Resources")
	void RemoveResources(FResourcesCost ResourcesToRemove);

	UFUNCTION(BlueprintCallable, Category="Resources")
	FResourcesCost GetResources();

	UFUNCTION(BlueprintCallable, Category="Resources")
	FResourcesCost GetMaxResource();
	int32 GetMaxResource(EResourceType Type);

	// ===== Mono Resource =====
	UFUNCTION(BlueprintCallable, Category="Resources")
	void SetResource(EResourceType Type, int32 Amount);
	
	UFUNCTION(BlueprintCallable, Category="Resources")
	void AddResource(EResourceType Type, int32 Amount);

	UFUNCTION(BlueprintCallable, Category="Resources")
	void RemoveResource(EResourceType Type, int32 Amount);

	UFUNCTION(BlueprintCallable, Category="Resources")
	int32 GetResource(EResourceType Type);


	
	UFUNCTION(BlueprintCallable, Category="Resources")
	bool GetStorageIsFull(EResourceType CheckResource = EResourceType::None);

	UFUNCTION(BlueprintCallable, Category="Resources")
	bool GetStorageIsEmpty(EResourceType CheckResource = EResourceType::None);

	UPROPERTY(BlueprintAssignable, Category="Resources")
	FOnResourcesChanged OnResourcesChanged;

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_CurrentResources();

	UFUNCTION(NetMulticast, reliable)
	void Multicast_UpdateResources(const FResourcesCost NewResources);

	UPROPERTY(ReplicatedUsing = OnRep_CurrentResources)
	FResourcesCost CurrentResources;

	UPROPERTY(EditAnywhere, Category="Resources")
	FResourcesCost MaxResource = FResourcesCost(100);

};
