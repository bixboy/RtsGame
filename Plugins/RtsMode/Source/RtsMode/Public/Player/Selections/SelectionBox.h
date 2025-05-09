﻿#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SelectionBox.generated.h"

class APlayerControllerRts;
class UBoxComponent;

UCLASS(Abstract)
class RTSMODE_API ASelectionBox : public AActor
{
	GENERATED_BODY()

public:
	ASelectionBox();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void Start(FVector Position, const FRotator Rotation);

	UFUNCTION()
	void End();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void Adjust() const;
	UFUNCTION()
	void Manage();

	UFUNCTION()
	void HandleHighlight(AActor* ActorInBox, const bool Highlight = true) const;

	UFUNCTION()
	virtual void OnBoxCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UPROPERTY()
	FVector StartLocation;
	UPROPERTY()
	FRotator StartRotation;

	UPROPERTY()
	TArray<AActor*> InBox;
	UPROPERTY()
	TArray<AActor*> CenterInBox;

protected:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = true))
	UBoxComponent* BoxComponent;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = true))
	UDecalComponent* Decal;

	UPROPERTY()
	bool BoxSelect;

	UPROPERTY()
	APlayerControllerRts* PlayerController;
};
