#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/DataRts.h"
#include "RtsResourcesComponent.generated.h"

UENUM(BlueprintType)
enum class EResourceType : uint8
{
	Wood UMETA(DisplayName = "Wood"),
	Food UMETA(DisplayName = "Food"),
	Metal UMETA(DisplayName = "Metal"),
	None
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnResourcesChanged, const FResourcesCost&, NewResources);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RTSGAME_API URtsResourcesComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URtsResourcesComponent();

	UFUNCTION(BlueprintCallable, Category="Resources")
	void AddResources(FResourcesCost NewResources);

	UFUNCTION(BlueprintCallable, Category="Resources")
	void RemoveResources(FResourcesCost ResourcesToRemove);

	UFUNCTION(BlueprintCallable, Category="Resources")
	FResourcesCost GetResources();

	UFUNCTION(BlueprintCallable, Category="Resources")
	bool GetStorageIsFull(EResourceType CheckResource = EResourceType::None);

	UFUNCTION(BlueprintCallable, Category="Resources")
	bool GetStorageIsEmpty(EResourceType CheckResource = EResourceType::None);

	UFUNCTION(BlueprintCallable, Category="Resources")
	FResourcesCost GetMaxResource();

	UPROPERTY(BlueprintAssignable, Category="Resources")
	FOnResourcesChanged OnResourcesChanged;

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_CurrentResources();

	UPROPERTY(ReplicatedUsing = OnRep_CurrentResources, EditAnywhere, Category="Resources")
	FResourcesCost CurrentResources;

	UPROPERTY(EditAnywhere, Category="Resources")
	FResourcesCost MaxResource = FResourcesCost(100);

};
