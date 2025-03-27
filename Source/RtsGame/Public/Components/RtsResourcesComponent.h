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

	UFUNCTION(BlueprintCallable, Category="Resources")
	void AddResources(FResourcesCost NewResources);

	UFUNCTION(BlueprintCallable, Category="Resources")
	void RemoveResources(FResourcesCost ResourcesToRemove);

	UFUNCTION(BlueprintCallable, Category="Resources")
	FResourcesCost GetResources();

	UPROPERTY(BlueprintAssignable, Category="Resources")
	FOnResourcesChanged OnResourcesChanged;

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	// Fonctions RPC 
	UFUNCTION(Server, Reliable)
	void Server_AddResources(FResourcesCost NewResources);

	UFUNCTION(Server, Reliable)
	void Server_RemoveResources(FResourcesCost ResourcesToRemove);

	UFUNCTION()
	void OnRep_CurrentResources();

	UPROPERTY(ReplicatedUsing = OnRep_CurrentResources, EditAnywhere, Category="Resources")
	FResourcesCost CurrentResources;

};
