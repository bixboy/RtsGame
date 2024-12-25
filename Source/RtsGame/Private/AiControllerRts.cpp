#include "RtsGame/Public/AiControllerRts.h"
#include "NavigationSystem.h"
#include "Data/AiData.h"
#include "SoldierRts.h"


// Setup Functions
#pragma region Setup Functions

AAiControllerRts::AAiControllerRts(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAiControllerRts::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	OwnerSoldier = Cast<ASoldierRts>(InPawn);
	if (OwnerSoldier)
	{
		OwnerSoldier->SetAIController(this);
		SetupVariables();
	}
}

void AAiControllerRts::SetupVariables()
{
	if (OwnerSoldier)
	{
		AttackCooldown = OwnerSoldier->GetAttackCooldown();
		AttackRange = OwnerSoldier->GetAttackRange();
		CombatBehavior = OwnerSoldier->GetCombatBehavior();
	}
}

#pragma endregion

void AAiControllerRts::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (HaveTargetAttack && OwnerSoldier)
	{
		if (CurrentCommand.Target)
		{
			float DistanceToTarget = FVector::Distance(OwnerSoldier->GetActorLocation(), CurrentCommand.Target->GetActorLocation());

			if (DistanceToTarget <= AttackRange && bCanAttack)
			{
				AttackTarget();
			}
			else if (DistanceToTarget > AttackRange && MoveComplete)
			{
				MoveComplete = false;
				MoveToActor(CurrentCommand.Target, 85.f);
			}	
		}
	}
}

// Movement
#pragma region Movement

void AAiControllerRts::CommandMove(const FCommandData& CommandData, bool Attack)
{
	CurrentCommand = CommandData;
	HaveTargetAttack = Attack;
	MoveComplete = false;
	InPatrol = false;
	
	if (HaveTargetAttack)
	{
		MoveToActor(CurrentCommand.Target, 85.f);
	}
	else
	{
		MoveToLocation(CurrentCommand.Location);
	}
}

void AAiControllerRts::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
	MoveComplete = true;
	OnReachedDestination.Broadcast(CurrentCommand);
	if (InPatrol)
	{
		InPatrol = false;
		CommandPatrol(CurrentCommand);
	}
}

#pragma endregion

// Attack
#pragma region Attack

void AAiControllerRts::AttackTarget()
{
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("Attacked By: %s"), *OwnerSoldier->GetName()));
	bCanAttack = false;
	
	GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, this, &AAiControllerRts::ResetAttack, AttackCooldown, false);
}

void AAiControllerRts::ResetAttack()
{
	bCanAttack = true;
}

void AAiControllerRts::StopAttack()
{
	GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
	bCanAttack = true;
	HaveTargetAttack = false;
}

ECombatBehavior AAiControllerRts::GetCombatBehavior() const
{
	return CombatBehavior;
}

FCommandData AAiControllerRts::GetCurrentCommand()
{
	return CurrentCommand;
}

bool AAiControllerRts::GetHaveTarget()
{
	return HaveTargetAttack;
}

void AAiControllerRts::SetHaveTarget(bool value)
{
	HaveTargetAttack = value;
}

#pragma endregion

// Patrol
#pragma region Patrol

void AAiControllerRts::CommandPatrol(const FCommandData& CommandData)
{
	CurrentCommand = CommandData;
	FVector OutLocation;
	bool bSuccess = UNavigationSystemV1::K2_GetRandomLocationInNavigableRadius(
		GetWorld(),
		CommandData.SourceLocation,
		OutLocation,
		CommandData.Radius,
		nullptr,
		nullptr
	);

	if (bSuccess)
	{
		FVector Destination = OutLocation;
		MoveToLocation(Destination, 20.f);
		InPatrol = true;
	}
}

#pragma endregion 

