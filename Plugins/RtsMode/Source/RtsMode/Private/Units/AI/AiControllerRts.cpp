#include "RtsMode/Public/Units/AI/AiControllerRts.h"
#include "NavigationSystem.h"
#include "Data/AiData.h"
#include "Units/SoldierRts.h"
#include "Components/WeaponMaster.h"

// ------------------- Setup Functions   ---------------------
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

	if (HasAuthority() && HaveTargetAttack && OwnerSoldier && CurrentCommand.Target)
	{
		const float DistanceToTarget = FVector::Distance(OwnerSoldier->GetActorLocation(), CurrentCommand.Target->GetActorLocation());

		// Attacking logic
		if (ShouldAttack(DistanceToTarget))
		{
			AttackTarget();
		}
		// Move towards target if out of range
		else if (ShouldMove(DistanceToTarget))
		{
			MoveComplete = false;
			MoveToActor(CurrentCommand.Target, CalculateMoveRange(DistanceToTarget));
		}
	}
}

bool AAiControllerRts::ShouldAttack(float DistanceToTarget) const
{
	bool bHasWeapon = OwnerSoldier->GetHaveWeapon();
	return (bHasWeapon && DistanceToTarget <= WeaponRange || !bHasWeapon && DistanceToTarget <= AttackRange) && bCanAttack;
}

bool AAiControllerRts::ShouldMove(float DistanceToTarget) const
{
	bool bHasWeapon = OwnerSoldier->GetHaveWeapon();
	return (bHasWeapon && DistanceToTarget > WeaponRange || !bHasWeapon && DistanceToTarget > AttackRange) && MoveComplete;
}

float AAiControllerRts::CalculateMoveRange(float DistanceToTarget) const
{
	return (OwnerSoldier->GetHaveWeapon() ? 200.f : AttackRange * 0.30f);
}

// ------------------- Movement   ---------------------
#pragma region Movement

void AAiControllerRts::CommandMove(const FCommandData& CommandData, bool Attack)
{
	CurrentCommand = CommandData;
	HaveTargetAttack = Attack;
	MoveComplete = false;
	InPatrol = false;

	if (HaveTargetAttack)
	{
		// If the soldier doesn't have a weapon, move closer to the target for melee
		float MoveRange = (OwnerSoldier->GetHaveWeapon()) ? 300.f : AttackRange * 0.30f;
		MoveToActor(CurrentCommand.Target, MoveRange);
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

// ------------------- Attack   ---------------------
#pragma region Attack

void AAiControllerRts::AttackTarget()
{
	if (!CurrentCommand.Target)
	{
		StopAttack();
		return;
	}

	// Attack logic based on weapon presence
	if (OwnerSoldier->GetCurrentWeapon())
	{
		OwnerSoldier->GetCurrentWeapon()->AIShoot(CurrentCommand.Target);
	}
	else
	{
		IDamageable::Execute_TakeDamage(CurrentCommand.Target, OwnerSoldier);
	}

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

// Getters
#pragma region Getters

ECombatBehavior AAiControllerRts::GetCombatBehavior() const
{
	return CombatBehavior;
}

FCommandData AAiControllerRts::GetCurrentCommand()
{
	return CurrentCommand;
}

bool AAiControllerRts::GetHaveTarget() const
{
	return HaveTargetAttack;
}

void AAiControllerRts::SetHaveTarget(bool Value)
{
	HaveTargetAttack = Value;
}

#pragma endregion

#pragma endregion

// ------------------- Patrol   ---------------------
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
		MoveToLocation(OutLocation, 20.f);
		InPatrol = true;
	}
}

#pragma endregion