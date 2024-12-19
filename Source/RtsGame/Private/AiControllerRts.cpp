#include "RtsGame/Public/AiControllerRts.h"

#include "AiData.h"
#include "SoldierRts.h"

AAiControllerRts::AAiControllerRts(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAiControllerRts::CommandMove(FCommandData CommandData)
{
	CurrentCommand = CommandData;

	MoveToLocation(CurrentCommand.Location);
}

void AAiControllerRts::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	OwnerSoldier = Cast<ASoldierRts>(InPawn);
	if (OwnerSoldier)
	{
		OwnerSoldier->SetAIController(this);
	}
}

void AAiControllerRts::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	OnReachedDestination.Broadcast(CurrentCommand);
}

