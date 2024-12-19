#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerControllerRts.generated.h"

struct FCommandData;
class UInputMappingContext;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSelectedUpdatedDelegate);

UCLASS()
class RTSGAME_API APlayerControllerRts : public APlayerController
{
	GENERATED_BODY()

public:
	APlayerControllerRts(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&) const override;

	UFUNCTION()
	FVector GetMousePositionOnTerrain() const;

	UFUNCTION()
	void Handle_Selection(AActor* ActorToSelect);
	void Handle_Selection(TArray<AActor*> ActorsToSelect);

	UFUNCTION()
	void CommandSelected(FCommandData CommandData);

protected:
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void Server_CommandSelected(FCommandData CommandData);

	UFUNCTION()
	bool ActorSelected(AActor* ActorToCheck) const;

	UFUNCTION(Server, Reliable)
	void Server_Select(AActor* ActorToSelect);

	UFUNCTION(Server, Reliable)
	void Server_Select_Group(const TArray<AActor*>& ActorsToSelect);

	UFUNCTION(Server, Reliable)
	void Server_DeSelect(AActor* ActorToDeSelect);

	UFUNCTION(Server, Reliable)
	void Server_ClearSelected();

	UFUNCTION()
	void OnRep_Selected();

	UPROPERTY(ReplicatedUsing = OnRep_Selected)
	TArray<AActor*> SelectedActors;

	UPROPERTY()
	FSelectedUpdatedDelegate OnSelectedUpdate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;
};
