// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AiData.h"
#include "InputActionValue.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/Selectable.h"
#include "SoldierRts.generated.h"

UCLASS()
class RTSGAME_API ASoldierRts : public APawn, public ISelectable
{
	GENERATED_BODY()

public:
	ASoldierRts();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(Blueprintable)
	void CommandStart(const FInputActionValue& Value);

private:
	//UFUNCTION()
	//FCommandData CreatCommandData(const ECommandType Type) const;

	UPROPERTY()
	TObjectPtr<AActor> PlayerOwner;
	UPROPERTY()
	FVector CommandLocation;

public:
	virtual void Select() override;
	virtual void Deselect() override;
	virtual void Highlight(const bool Highlight) override;

	UPROPERTY()
	bool Selected;
};
