﻿#pragma once

#include "CoreMinimal.h"
#include "Data/AiData.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/Damageable.h"
#include "Interfaces/Selectable.h"
#include "SoldierRts.generated.h"

class UWeaponMaster;
class USphereComponent;
class UCommandComponent;
class AAiControllerRts;
class UCharacterMovementComponent;
class APlayerControllerRts;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBehaviorUpdatedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSelectedDelegate, bool, bIsSelected);

UCLASS(Blueprintable)
class RTSMODE_API ASoldierRts : public ACharacter, public ISelectable, public IDamageable
{
	GENERATED_BODY()

public:
	ASoldierRts(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	virtual FCommandData GetCurrentCommand_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UCommandComponent* GetCommandComponent() const;

protected:
	UFUNCTION()
	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION()
	virtual void Destroyed() override;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UCommandComponent> CommandComp;

// AI controller
#pragma region AI Controller
	
public:
	UFUNCTION()
	void SetAIController(AAiControllerRts* AiController);
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	AAiControllerRts* GetAiController() const;
	
protected:
	UPROPERTY()
	TObjectPtr<AAiControllerRts> AIController;
	
	UPROPERTY(EditAnywhere, Category = "Settings")
	TSubclassOf<AAiControllerRts> AiControllerRtsClass;

#pragma endregion	

// Selection	
#pragma region Selection
	
public:
	/*- Function -*/
	virtual void Select() override;
	virtual void Deselect() override;
	virtual void Highlight(const bool Highlight) override;
	
	UFUNCTION()
	virtual bool GetIsSelected_Implementation() override;

	UPROPERTY()
	FSelectedDelegate OnSelected;
	
protected:
	/*- Variables -*/
	UPROPERTY()
	bool Selected;
	
	UPROPERTY()
	TObjectPtr<APlayerControllerRts> PlayerOwner;

#pragma endregion

// Movement	
#pragma region Movement
public:
	UFUNCTION()
	virtual void CommandMove_Implementation(FCommandData CommandData) override;

#pragma endregion	

// Attack
#pragma region Attack

public:
	/*- Interface -*/
	virtual void TakeDamage_Implementation(AActor* DamageOwner) override;
	
	virtual bool GetIsInAttack_Implementation() override;

	virtual bool GetCanAttack_Implementation() override;
	
	/*- Getter -*/
	UFUNCTION()
	float GetAttackRange() const;

	UFUNCTION()
	float GetAttackCooldown() const;

	UFUNCTION()
	ECombatBehavior GetCombatBehavior() const;
	
protected:
	
	/*- Function -*/
	UFUNCTION()
	void OnAreaAttackBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnAreaAttackEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void SetBehavior_Implementation(const ECombatBehavior NewBehavior) override;

	UFUNCTION()
	virtual ECombatBehavior GetBehavior_Implementation() override;

	UFUNCTION()
	virtual void OnStartAttack(AActor* Target);

	UFUNCTION()
	void UpdateActorsInArea();

	UFUNCTION()
	void OnRep_CombatBehavior();
	
	/*- Variables -*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> AreaAttack;

	UPROPERTY(EditAnywhere, Category = "Settings|Attack")
	bool bCanAttack = true;
	
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_CombatBehavior, meta = (EditCondition = "bCanAttack", EditConditionHides), Category = "Settings|Attack")
	ECombatBehavior CombatBehavior = ECombatBehavior::Passive;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bCanAttack", EditConditionHides), Category = "Settings|Attack")
	float AttackCooldown = 1.5f;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bCanAttack", EditConditionHides), Category = "Settings|Attack")
	float AttackRange = 200.f;
	
	UPROPERTY()
	TArray<AActor*> ActorsInRange;

	UPROPERTY()
	TArray<AActor*> AllyInRange;

	UPROPERTY()
	FBehaviorUpdatedDelegate OnBehaviorUpdate;

#pragma endregion

// Team	
#pragma region Team
public:
	UFUNCTION(BlueprintCallable)
	virtual ETeams GetCurrentTeam_Implementation() override;

protected:
	UPROPERTY(EditAnywhere, Category = "Settings|Team")
	ETeams CurrentTeam = ETeams::Clone;

#pragma endregion

// Weapons
#pragma region Weapons
	
public:
	UFUNCTION()
	UWeaponMaster* GetCurrentWeapon();
	
	UFUNCTION()
	bool GetHaveWeapon();
	
protected:
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bCanAttack", EditConditionHides), Category = "Settings|Attack")
	TSubclassOf<UWeaponMaster> WeaponClass;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	UWeaponMaster* CurrentWeapon;
	
	UPROPERTY(BlueprintReadWrite)
	bool HaveWeapon;

#pragma endregion
};
