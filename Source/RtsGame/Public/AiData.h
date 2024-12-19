#pragma once

#include "AIData.generated.h"

UENUM(BlueprintType)
enum ECommandType
{
	CommandMove,
	CommandMoveFast,
	CommandMoveSlow,
	CommandAttack
};

USTRUCT(BlueprintType)
struct FCommandData
{
	GENERATED_BODY()

	// Initialisation
	FCommandData()
	: Location(FVector::ZeroVector),
		Rotation(FRotator::ZeroRotator),
		Type(ECommandType::CommandMove),
		Target(nullptr) {}

	// Assignation des parametres 
	FCommandData(const FVector InLocation, const FRotator InRotation, const ECommandType InType, AActor* InTarget = nullptr)
	:	Location(InLocation),
		Rotation(InRotation),
		Type(InType),
		Target(InTarget) {}


	// Variables
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Location;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator Rotation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<ECommandType> Type;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AActor* Target;
};
