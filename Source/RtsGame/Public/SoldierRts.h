// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AiData.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/Selectable.h"
#include "SoldierRts.generated.h"

class AAiControllerRts;
class UCharacterMovementComponent;
class APlayerControllerRts;

UCLASS(Blueprintable)
class RTSGAME_API ASoldierRts : public ACharacter, public ISelectable
{
	GENERATED_BODY()

public:
	ASoldierRts();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY()
	TObjectPtr<APlayerControllerRts> PlayerOwner;

public:
	virtual void Select() override;
	virtual void Deselect() override;
	virtual void Highlight(const bool Highlight) override;

	UPROPERTY()
	bool Selected;

	UFUNCTION()
	void CommandMoveToLocation(const FCommandData CommandData);
	UFUNCTION()
	void SetAIController(AAiControllerRts* AiController);

protected:
	UFUNCTION()
	void CommandMove(const FCommandData CommandData);
	UFUNCTION()
	void DestinationReached(const FCommandData CommandData);

	UFUNCTION()
	void SetWalk() const;
	UFUNCTION()
	void SetRun() const;
	UFUNCTION()
	void SetSprint() const;

	UFUNCTION()
	void SetOrientation(const float DeltaTime);
	UFUNCTION()
	bool IsOrientated() const;

	
	UPROPERTY(EditAnywhere)
	float MaxSpeed = 100.f;
	UPROPERTY()
	UCharacterMovementComponent* CharaMovementComp;

	UPROPERTY()
	FRotator TargetOrientation;

	UPROPERTY()
	uint8 ShouldOrientate;

	UPROPERTY()
	AAiControllerRts* AIController;
};
