#pragma once

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

UCLASS(Blueprintable)
class RTSMODE_API ASoldierRts : public ACharacter, public ISelectable, public IDamageable
{
	GENERATED_BODY()

public:
	ASoldierRts();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UCommandComponent* GetCommandComponent() const;

protected:
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
	
	UPROPERTY(EditAnywhere, Category = "Settings|DefaultValue")
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
	bool GetIsSelected() const;
	
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
	
	/*- Getter -*/
	float GetAttackRange() const;
	float GetAttackCooldown() const;
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
	
	/*- Variables -*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> AreaAttack;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Attack", ReplicatedUsing = OnRep_CombatBehavior)
	ECombatBehavior CombatBehavior = ECombatBehavior::Passive;
	UPROPERTY(EditAnywhere, Category = "Settings|Attack")
	float AttackCooldown = 1.5f;
	UPROPERTY(EditAnywhere, Category = "Settings|Attack")
	float AttackRange = 200.f;
	
	UPROPERTY()
	TArray<AActor*> ActorsInRange;
	UFUNCTION()
	void UpdateActorsInArea();

	UPROPERTY()
	TArray<AActor*> AllyInRange;

	virtual void BeginDestroy() override;

	UPROPERTY()
	FBehaviorUpdatedDelegate OnBehaviorUpdate;
	UFUNCTION()
	void OnRep_CombatBehavior();

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
	UPROPERTY(EditAnywhere, Category = "Settings|Weapons")
	TSubclassOf<UWeaponMaster> WeaponClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	UWeaponMaster* CurrentWeapon;
	
	UPROPERTY(BlueprintReadWrite)
	bool HaveWeapon;

#pragma endregion
};
