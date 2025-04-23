#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;


UCLASS()
class RTSGAME_API AProjectile : public AActor
{
	GENERATED_BODY()

public:	
	AProjectile();

	UFUNCTION()
	void LaunchProjectile(FVector InVelocity);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	// Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float Damage = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float LifeSpan = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bDestroyOnImpact = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	UParticleSystem* ImpactEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	USoundBase* ImpactSound;

	// Components
	UPROPERTY(EditAnywhere, Category = "Components")
	USphereComponent* CollisionComponent;

	UPROPERTY(EditAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, Category = "Components")
	UProjectileMovementComponent* ProjectileMovement;
};
