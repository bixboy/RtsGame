#pragma once
#include "CoreMinimal.h"
#include "StructureBase.h"
#include "DefenceStructure.generated.h"


UCLASS()
class RTSGAME_API ADefenceStructure : public AStructureBase
{
	GENERATED_BODY()

public:
	ADefenceStructure();

protected:
	virtual void BeginPlay() override;

	// ----- Detection zone -----
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Defense")
	USphereComponent* DetectionSphere;

	UPROPERTY()
	TArray<AActor*> TargetsInRange;

	// ----- Attack settings -----
	UPROPERTY(EditAnywhere, Category="Settings|Defence")
	float AttackDamage = 20.f;

	UPROPERTY(EditAnywhere, Category="Settings|Defence")
	float AttackRate = 1.0f;

	FTimerHandle AttackTimerHandle;

	// ----- Callbacks -----
	UFUNCTION()
	void OnDetectionBegin(UPrimitiveComponent* Overlapped, AActor* Other,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& Sweep);

	UFUNCTION()
	void OnDetectionEnd(UPrimitiveComponent* Overlapped, AActor* Other,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// ----- Attack logic -----
	UFUNCTION()
	void PerformAttack();

	UFUNCTION()
	void StartAttacking();

	UFUNCTION()
	void StopAttacking();

	UFUNCTION(BlueprintImplementableEvent)
	void DefenceAttack(AActor* Target);
};
