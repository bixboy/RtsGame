#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Data/AiData.h"
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

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void CommandMove(const FCommandData& CommandData, bool Attack = false);

	UPROPERTY()
	FReachedDestinationDelegate OnReachedDestination;

	UFUNCTION()
	void StopAttack();
	UFUNCTION()
	void SetupVariables();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	ECombatBehavior GetCombatBehavior() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FCommandData GetCurrentCommand();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetHaveTarget();
	UFUNCTION(BlueprintCallable)
	void SetHaveTarget(bool value);

protected:
	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY()
	ASoldierRts* OwnerSoldier;

	UPROPERTY()
	FCommandData CurrentCommand;

	/*- Movement -*/
	bool MoveComplete;

	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	/*- Attack -*/
	FTimerHandle AttackTimerHandle;
	
	bool bCanAttack = true;
	bool HaveTargetAttack;

	UPROPERTY()
	ECombatBehavior CombatBehavior = ECombatBehavior::Neutral;
	UPROPERTY()
	float AttackCooldown = 1.5f;
	UPROPERTY()
	float AttackRange = 200.f;

	UFUNCTION()
	void AttackTarget();
	UFUNCTION()
	void ResetAttack();
};
