#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChangedSignature, float, NewHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealedSignature, float, NewHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTakeDamageSignature, float, DamageValue, FVector, ImpactPoint);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RTSGAME_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();
	
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void UpdateHealth(float Delta);

	UFUNCTION()
	void RegenerateHealth();

	UFUNCTION()
	void OnRep_CurrentHealth();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnDamageTaken(float DamageValue, FVector ImpactPoint);


	// Health Variables
	UPROPERTY(EditAnywhere,  BlueprintReadOnly, Category = "Settings|Health")
	float MaxHealth = 50.f;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentHealth, Category = "Settings|Health")
	float CurrentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Settings|Health")
	bool bIsInvulnerable = false;

	UPROPERTY()
	float LastDamageTime = 0.f;

	UPROPERTY()
	FTimerHandle InvincibilityTimerHandle;

	// --- Régénération de santé ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Health|Regen")
	bool bEnableHealthRegen = true;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Health|Regen")
	float RegenRate = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Health|Regen")
	float RegenDelay = 3.f;

	UPROPERTY()
	FTimerHandle RegenTimerHandle;

	// --- Effets visuels et audio ---
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Effects")
	UParticleSystem* DamageParticleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Effects")
	USoundBase* DamageSound;

public:
	// Health Function
	UFUNCTION(BlueprintCallable, Category = "Health")
	void TakeDamage(float DamageAmount, FVector ImpactPoint = FVector::ZeroVector);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void Heal(float HealAmount);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void ResetHealth();

	UFUNCTION(BlueprintCallable, Category = "Health")
	bool IsAlive() const { return CurrentHealth > 0.f; }

	UFUNCTION(BlueprintCallable, Category = "Health")
	void ActivateTemporaryInvincibility(float Duration);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void DeactivateInvincibility();


	// Delegate
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDeathSignature OnDeath;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealedSignature OnHealed;
	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnTakeDamageSignature OnTakeDamage;
};
