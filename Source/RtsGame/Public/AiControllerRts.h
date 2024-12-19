#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AiData.h"
#include "AiControllerRts.generated.h"

class ASoldierRts;
struct FCommandData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FReachedDestinationDelegate, const FCommandData, CommandData);

UCLASS()
class RTSGAME_API AAiControllerRts : public AAIController
{
	GENERATED_BODY()

public:
	AAiControllerRts(const FObjectInitializer& ObjectInitializer);

	UFUNCTION()
	void CommandMove(FCommandData CommandData);

	UPROPERTY()
	FReachedDestinationDelegate OnReachedDestination;

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	UPROPERTY()
	ASoldierRts* OwnerSoldier;

	UPROPERTY()
	FCommandData CurrentCommand;
	
};
