﻿#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CommandComponent.generated.h"


class ASoldierRts;
class AAiControllerRts;
class UCharacterMovementComponent;
struct FCommandData;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RTSGAME_API UCommandComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCommandComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void CommandMoveToLocation(const FCommandData CommandData);
	void SetOwnerAIController(AAiControllerRts* Cast);

	FVector GetCommandLocation() const;

private:
#pragma region Command Functions
	
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

	UFUNCTION(Client, Reliable)
	void Client_SetMoveMarker(const FVector Location, const FCommandData CommandData);

	UFUNCTION()
	FTransform GetPositionTransform(const FVector Position) const;

#pragma endregion

#pragma region Variables

	UPROPERTY()
	TObjectPtr<ASoldierRts> OwnerActor;
	
	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> OwnerCharaMovementComp;
	UPROPERTY(EditAnywhere)
	TObjectPtr<AAiControllerRts> OwnerAIController;

	UPROPERTY(EditAnywhere, Category = "Command|Settings")
	float MaxSpeed = 100.f;
	UPROPERTY()
	FVector TargetLocation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true), Category = "Command|Settings")
	TSubclassOf<AActor> MoveMarkerClass;
	UPROPERTY()
	TObjectPtr<AActor> MoveMarker;

	UPROPERTY()
	FRotator TargetOrientation;
	UPROPERTY()
	uint8 ShouldOrientate;

	UPROPERTY()
	bool HaveTargetAttack;
	
#pragma endregion	
};