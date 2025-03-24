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

	UFUNCTION()
	void OnNewResources(const FResourcesCost& NewResources);

	UPROPERTY()
	URtsComponent* RtsComponent;

	UPROPERTY()
	URtsResourcesComponent* ResourcesComponent;
};
