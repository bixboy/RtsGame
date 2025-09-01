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
	UFUNCTION(BlueprintCallable, Category="Resources|Multi")
	void SetResources(FResourcesCost NewResources);

	UFUNCTION(BlueprintCallable, Category="Resources|Multi")
	void AddResources(FResourcesCost NewResources);

	UFUNCTION(BlueprintCallable, Category="Resources|Multi")
	void RemoveResources(FResourcesCost ResourcesToRemove);

	UFUNCTION(BlueprintCallable, Category="Resources|Multi")
	FResourcesCost GetResources() const;

	UFUNCTION(BlueprintCallable, Category="Resources|Multi")
	FResourcesCost GetMaxResources() const;
	

	// ===== Single Resource =====
	UFUNCTION(BlueprintCallable, Category="Resources|Single")
	void SetResource(EResourceType Type, int32 Amount);

	UFUNCTION(BlueprintCallable, Category="Resources|Single")
	void AddResource(EResourceType Type, int32 Amount);

	UFUNCTION(BlueprintCallable, Category="Resources|Single")
	void RemoveResource(EResourceType Type, int32 Amount);

	UFUNCTION(BlueprintCallable, Category="Resources|Single")
	int32 GetResource(EResourceType Type);

	UFUNCTION(BlueprintCallable, Category="Resources|Single")
	int32 GetMaxResource(EResourceType Type);
	

	// Event broadcast quand les ressources changent
	UPROPERTY(BlueprintAssignable, Category="Resources")
	FOnResourcesChanged OnResourcesChanged;
	

	UFUNCTION(BlueprintCallable, Category="Resources|Multi")
	bool GetStorageIsFull(EResourceType Type = EResourceType::None);

	UFUNCTION(BlueprintCallable, Category="Resources|Multi")
	bool GetStorageIsEmpty(EResourceType Type = EResourceType::None);

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_CurrentResources();
	
	UFUNCTION()
	void ApplyResourceChange(const FResourcesCost& NewResources);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ApplyResourceChange(const FResourcesCost& NewResources);

	UPROPERTY(ReplicatedUsing=OnRep_CurrentResources, VisibleAnywhere, Category="Resources")
	FResourcesCost CurrentResources;

	UPROPERTY(EditAnywhere, Category="Resources")
	FResourcesCost MaxResource = FResourcesCost(100);

};
