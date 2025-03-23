#include "Components/HealthComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


// ----------------- Setup -----------------
#pragma region Setup

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	SetIsReplicatedByDefault(true);
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, CurrentHealth);
	DOREPLIFETIME(UHealthComponent, bIsInvulnerable);
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		CurrentHealth = MaxHealth;
		LastDamageTime = 0.f;

		if (bEnableHealthRegen)
		{
			GetWorld()->GetTimerManager().SetTimer(RegenTimerHandle, this, &UHealthComponent::RegenerateHealth, 0.1f, true);
		}
	}
}

#pragma endregion


// ----------------- Health -----------------

void UHealthComponent::UpdateHealth(float Delta)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
	if (bIsInvulnerable && Delta < 0.f) return;
    
	float OldHealth = CurrentHealth;
	CurrentHealth = FMath::Clamp(CurrentHealth + Delta, 0.f, MaxHealth);
	OnHealthChanged.Broadcast(CurrentHealth);
	
	// Sinon, c’est un soin
	if (Delta < 0.f)
	{
		LastDamageTime = GetWorld()->GetTimeSeconds();
	}
	else if (Delta > 0.f)
	{
		OnHealed.Broadcast(CurrentHealth);
	}

	// Si la santé tombe à 0, on déclenche la mort
	if (CurrentHealth <= 0.f && OldHealth > 0.f)
	{
		OnDeath.Broadcast();
	}
}

void UHealthComponent::ResetHealth()
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
	
	CurrentHealth = MaxHealth;
	OnHealthChanged.Broadcast(CurrentHealth);
}

void UHealthComponent::OnRep_CurrentHealth()
{
	OnHealthChanged.Broadcast(CurrentHealth);
	if (CurrentHealth <= 0.f)
	{
		OnDeath.Broadcast();
	}
}


// ----------------- Healing -----------------

void UHealthComponent::RegenerateHealth()
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	float CurrentTime = GetWorld()->GetTimeSeconds();
	if ((CurrentTime - LastDamageTime) >= RegenDelay && CurrentHealth < MaxHealth)
	{
		float HealAmount = RegenRate * 0.1f;
		UpdateHealth(HealAmount);
	}
}

void UHealthComponent::Heal(float HealAmount)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || HealAmount <= 0.f || CurrentHealth <= 0.f) return;
	
	UpdateHealth(HealAmount);
}


// ----------------- Take Damage -----------------

void UHealthComponent::TakeDamage(float DamageAmount, FVector ImpactPoint)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || DamageAmount <= 0.f) return;
	
	UpdateHealth(-DamageAmount);
	
	OnTakeDamage.Broadcast(DamageAmount, ImpactPoint);
	Multicast_OnDamageTaken(DamageAmount, ImpactPoint);
}

void UHealthComponent::Multicast_OnDamageTaken_Implementation(float DamageValue, FVector ImpactPoint)
{
	OnTakeDamage.Broadcast(DamageValue, ImpactPoint);

	// l'effet visuel
	if (DamageParticleEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DamageParticleEffect, ImpactPoint);
	}
	// l'effet audio
	if (DamageSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), DamageSound, ImpactPoint);
	}
}


// ----------------- Invincibility -----------------

void UHealthComponent::ActivateTemporaryInvincibility(float Duration)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	bIsInvulnerable = true;
	GetWorld()->GetTimerManager().SetTimer(InvincibilityTimerHandle, this, &UHealthComponent::DeactivateInvincibility, Duration, false);
}

void UHealthComponent::DeactivateInvincibility()
{
	bIsInvulnerable = false;
}