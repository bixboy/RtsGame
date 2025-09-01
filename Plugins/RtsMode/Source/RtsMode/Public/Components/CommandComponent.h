﻿#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/AiData.h"
#include "CommandComponent.generated.h"


class ASoldierRts;
class AAiControllerRts;
class UCharacterMovementComponent;
struct FCommandData;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RTSMODE_API UCommandComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCommandComponent();

	UFUNCTION()
	void CommandMoveToLocation(const FCommandData& CommandData);
	
	UFUNCTION()
	void SetOwnerAIController(AAiControllerRts* Cast);

	UFUNCTION()
	FVector GetCommandLocation() const;

	UFUNCTION()
	FCommandData GetCurrentCommand() const;

protected:
	
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;;
	
	UFUNCTION()
	void InitializeMovementComponent() const;

// Commands
#pragma region Command Functions
private:
	UFUNCTION()
	void CommandPatrol(const FCommandData CommandData);
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

	UFUNCTION()
	FTransform GetPositionTransform(const FVector Position) const;

	UPROPERTY()
	FCommandData CurrentCommand;

#pragma endregion


// Variables
#pragma region Variables
protected:
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


// Move Marker	
#pragma region Move Marker
public:
	UFUNCTION(Client, Reliable)
	void ShowMoveMarker(bool bIsSelected);
	
protected:
	UFUNCTION()
	void CreatMoveMarker();

	UFUNCTION(NetMulticast, Reliable)
	void SetMoveMarker(const FVector Location, const FCommandData CommandData);

#pragma endregion	
};
