#pragma once
#include "CoreMinimal.h"
#include "Data/DataRts.h"
#include "Player/PlayerControllerRts.h"
#include "RtsPlayerController.generated.h"

class URtsResourcesComponent;
class URtsComponent;


UCLASS()
class RTSGAME_API ARtsPlayerController : public APlayerControllerRts
{
	GENERATED_BODY()
	
public:
	ARtsPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnNewResources(const FResourcesCost& NewResources);

	UFUNCTION()
	void AddResource(FResourcesCost NewResource);

	UFUNCTION()
	void RemoveResource(FResourcesCost RemoveResource);

	UPROPERTY(EditAnywhere)
	URtsComponent* RtsComponent;

	UPROPERTY(EditAnywhere)
	URtsResourcesComponent* ResourcesComponent;
};
