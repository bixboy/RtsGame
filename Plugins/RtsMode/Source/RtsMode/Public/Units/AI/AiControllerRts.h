#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Data/AiData.h"
#include "AiControllerRts.generated.h"

class ASoldierRts;
struct FCommandData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FReachedDestinationDelegate, const FCommandData, CommandData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNewDestinationDelegate, const FCommandData, CommandData);

UCLASS()
class RTSMODE_API AAiControllerRts : public AAIController
{
	GENERATED_BODY()

public:
	AAiControllerRts(const FObjectInitializer& ObjectInitializer);
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	bool ShouldAttack(float DistanceToTarget) const;
	
	UFUNCTION()
	bool ShouldMove(float DistanceToTarget) const;
	
	UFUNCTION()
	float CalculateMoveRange(float DistanceToTarget) const;

	UFUNCTION()
	void CommandMove(const FCommandData& CommandData, bool Attack = false);

	UFUNCTION()
	void CommandPatrol(const FCommandData& CommandData);

	UPROPERTY()
	FReachedDestinationDelegate OnReachedDestination;

	UPROPERTY()
	FOnNewDestinationDelegate OnNewDestination;

	UFUNCTION()
	void SetupVariables();


protected:
	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY()
	ASoldierRts* OwnerSoldier;

	UPROPERTY()
	FCommandData CurrentCommand;

	// Movement control flags
	UPROPERTY()
	bool MoveComplete = false;
	UPROPERTY()
	bool InPatrol = false;

	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

// Attack logic
#pragma region Attack
protected:
	UPROPERTY()
	FTimerHandle AttackTimerHandle;

	UPROPERTY()
	ECombatBehavior CombatBehavior = ECombatBehavior::Neutral;
	
	UPROPERTY()
	bool bCanAttack = true;
	UPROPERTY()
	bool HaveTargetAttack = false;

	// Weapon attack settings
	UPROPERTY(EditAnywhere, Category = "Settings|Weapon Values")
	float AttackCooldown = 1.5f;
	UPROPERTY(EditAnywhere, Category = "Settings|Weapon Values")
	float AttackRange = 200.f;
	UPROPERTY(EditAnywhere, Category = "Settings|Weapon Values")
	float WeaponRange = 400.f;

	// Attack related functions
	UFUNCTION()
	void AttackTarget();
	UFUNCTION()
	void ResetAttack();

public:
	// Getters and setters for attack state
	bool GetHaveTarget() const;
	UFUNCTION(BlueprintCallable)
	void SetHaveTarget(bool value);
	UFUNCTION()
	void StopAttack();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	ECombatBehavior GetCombatBehavior() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FCommandData GetCurrentCommand();

#pragma endregion	
};