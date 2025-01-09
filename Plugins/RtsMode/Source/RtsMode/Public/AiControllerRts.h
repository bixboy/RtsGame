#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Data/AiData.h"
#include "AiControllerRts.generated.h"

class ASoldierRts;
struct FCommandData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FReachedDestinationDelegate, const FCommandData, CommandData);

UCLASS()
class RTSMODE_API AAiControllerRts : public AAIController
{
	GENERATED_BODY()

public:
	AAiControllerRts(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void CommandMove(const FCommandData& CommandData, bool Attack = false);
	UFUNCTION()
	void CommandPatrol(const FCommandData& CommandData);

	UPROPERTY()
	FReachedDestinationDelegate OnReachedDestination;
	
	UFUNCTION()
	void SetupVariables();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FCommandData GetCurrentCommand();

protected:
	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY()
	ASoldierRts* OwnerSoldier;

	UPROPERTY()
	FCommandData CurrentCommand;

	/*- Movement -*/
	bool MoveComplete;
	bool InPatrol;

	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

// Attack
#pragma region Attack
protected:	
	/*- Variables -*/
	FTimerHandle AttackTimerHandle;
	
	ECombatBehavior CombatBehavior = ECombatBehavior::Neutral;
	
	bool bCanAttack = true;
	bool HaveTargetAttack;
	
	UPROPERTY(EditAnywhere, Category = "Settings|Weapon Values")
	float AttackCooldown = 1.5f;
	UPROPERTY(EditAnywhere, Category = "Settings|Weapon Values")
	float AttackRange = 200.f;
	UPROPERTY(EditAnywhere, Category = "Settings|Weapon Values")
	float WeaponRange = 400.f;

	/*- Functions -*/
	UFUNCTION()
	void AttackTarget();
	UFUNCTION()
	void ResetAttack();

public:
	UFUNCTION(BlueprintCallable)
	void SetHaveTarget(bool value);
	UFUNCTION()
	void StopAttack();

	/*- Getter -*/
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetHaveTarget();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	ECombatBehavior GetCombatBehavior() const;
	
#pragma endregion	
};
