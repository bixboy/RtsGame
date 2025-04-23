﻿#pragma once
#include "CoreMinimal.h"
#include "AIController.h"
#include "Data/AiData.h"
#include "AiControllerRts.generated.h"

class ASoldierRts;
struct FCommandData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FReachedDestination, const FCommandData, CommandData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNewDestination, const FCommandData, CommandData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStartAttack, AActor*, Target);


UCLASS()
class RTSMODE_API AAiControllerRts : public AAIController
{
	GENERATED_BODY()

public:
	AAiControllerRts();
	virtual void Tick(float DeltaSeconds) override;
	virtual void OnPossess(APawn* InPawn) override;
	
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	UFUNCTION(BlueprintCallable, Category="AI")
	void CommandMove(const FCommandData Cmd, bool bAttack = false);

	UFUNCTION(BlueprintCallable, Category="AI")
	void CommandPatrol(const FCommandData Cmd);

	UFUNCTION(BlueprintCallable, Category="AI")
	void ResetAttack();

	UFUNCTION(BlueprintCallable, Category="AI")
	void StopAttack();

	// Delegates
	UPROPERTY(BlueprintAssignable, Category="AI")
	FOnNewDestination OnNewDestination;

	UPROPERTY(BlueprintAssignable, Category="AI")
	FReachedDestination OnReachedDestination;

	UPROPERTY(BlueprintAssignable, Category="AI")
	FOnStartAttack OnStartAttack;

	/** Accessors */
	UFUNCTION(BlueprintPure, Category="AI")
	FCommandData GetCurrentCommand() const { return CurrentCommand; }
	
	UFUNCTION(BlueprintPure, Category="AI")
	bool IsMoveComplete() const { return bMoveComplete; }

	// Attack
	UFUNCTION(BlueprintPure, Category="AI")
	bool HasAttackTarget() const { return bAttackTarget; }

	UFUNCTION(BlueprintPure, Category="AI")
	bool CanAttack() const { return bCanAttack; }

	UFUNCTION(BlueprintCallable, Category="AI")
	void SetAttackTarget(bool bAttack) { bAttackTarget = bAttack; }

	UFUNCTION(BlueprintCallable, Category="AI")
	ECombatBehavior GetCombatBehavior() const { return CombatBehavior; }

	UFUNCTION(BlueprintCallable, Category="AI")
	void SetupVariables();

protected:
	/** AI settings */
	UPROPERTY(EditAnywhere, Category="AI")
	float MeleeApproachFactor = 0.3f;

	UPROPERTY(EditAnywhere, Category="AI")
	float RangedStopDistance = 200.f;

	UPROPERTY(EditAnywhere, Category="AI")
	float AttackCooldown = 1.f;

private:
	// Variables
	UPROPERTY() ASoldierRts*       Soldier = nullptr;
	UPROPERTY() FCommandData       CurrentCommand;
	
	UPROPERTY() bool               bMoveComplete = true;
	UPROPERTY() bool               bPatrolling = false;
	
	UPROPERTY() bool               bAttackTarget = false;
	UPROPERTY() bool               bCanAttack = true;
	UPROPERTY() FTimerHandle       AttackTimer;
	UPROPERTY() ECombatBehavior    CombatBehavior;
	
	// Functions
	UFUNCTION() float GetAcceptanceRadius() const;
	UFUNCTION() bool  ShouldApproach() const;

	UFUNCTION() float GetDistanceToTarget() const;
	UFUNCTION() bool  ShouldAttack() const;
	UFUNCTION() void  PerformAttack();
	
	UFUNCTION() void  StartPatrol();
};