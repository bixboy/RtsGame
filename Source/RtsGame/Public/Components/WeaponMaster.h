#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "WeaponMaster.generated.h"

UCLASS()
class RTSGAME_API UWeaponMaster : public USkeletalMeshComponent
{
	GENERATED_BODY()

protected:
	/*- Function -*/
	UFUNCTION(BlueprintCallable)
	FVector GetDirection();
	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetShootTrace(FVector Direction);

	UFUNCTION(BlueprintCallable)
	void SpawnBullet();
	
	UFUNCTION()
	FHitResult PerformSingleLineTrace(const FVector& Start, const FVector& End, ECollisionChannel TraceChannel) const;

	/*- Variables -*/
	UPROPERTY(EditAnywhere, Category = "Settings|Weapon Values")
	TObjectPtr<USkeletalMesh> WeaponMesh;
	UPROPERTY(EditAnywhere, Category = "Settings|Weapon Values")
	TSubclassOf<AActor> BulletClass;
	
	UPROPERTY(EditAnywhere, Category = "Settings|Weapon Values")
	float WeaponRange = 1000.f;
	UPROPERTY(EditAnywhere, Category = "Settings|Weapon Values")
	bool HasAiming;
	
};

