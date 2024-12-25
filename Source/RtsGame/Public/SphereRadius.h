﻿#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SphereRadius.generated.h"

class APlayerControllerRts;
class USphereComponent;

UCLASS()
class RTSGAME_API ASphereRadius : public AActor
{
	GENERATED_BODY()

public:
	ASphereRadius();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	float Adjust() const;

public:
	UFUNCTION()
	void Start(FVector Position, const FRotator Rotation);
	UFUNCTION()
	void End();

	UFUNCTION()
	float GetRadius() const;

	UPROPERTY()
	FVector StartLocation;
	UPROPERTY()
	FRotator StartRotation;

private:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = true))
	UDecalComponent* Decal;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = true))
	USphereComponent* SphereComponent;

	UPROPERTY()
	bool SphereEnable;
	UPROPERTY()
	float CurrentRadius;

	UPROPERTY()
	APlayerControllerRts* PlayerController;
};
