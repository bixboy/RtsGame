#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "WeaponMaster.generated.h"

UCLASS()
class RTSGAME_API UWeaponMaster : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	UWeaponMaster();
	virtual void BeginPlay() override;

protected:
	/*- Function -*/
	UFUNCTION(BlueprintCallable)
	FVector GetDirection();
	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetShootTrace(FVector Direction);

	UFUNCTION(BlueprintCallable)
	void SpawnBullet();
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SetHasAiming_Server(bool Value);
	
	UFUNCTION()
	FHitResult PerformSingleLineTrace(const FVector& Start, const FVector& End, ECollisionChannel TraceChannel) const;

	/*- Variables -*/
	UPROPERTY(EditAnywhere, Category = "Settings|Weapon Values")
	TObjectPtr<USkeletalMesh> WeaponMesh;
	UPROPERTY(EditAnywhere, Category = "Settings|Weapon Values")
	TSubclassOf<AActor> BulletClass;
	
	UPROPERTY(EditAnywhere, Category = "Settings|Weapon Values")
	float WeaponRange = 1000.f;
	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Settings|Weapon Values")
	bool HasAiming = false;

	UPROPERTY()
	TArray<FVector> StartEndPoints;
	UPROPERTY()
	APlayerController* PlayerController;

	/*- Server Replication -*/
	UFUNCTION(Server, Reliable)
	void SpawnBullet_Server();
	UFUNCTION(Server, Reliable)
	void GetShootTrace_Server(const FVector Direction);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};

