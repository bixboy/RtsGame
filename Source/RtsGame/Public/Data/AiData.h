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

UENUM(BlueprintType)
enum EFormation
{
	Line	UMETA(DisplayName = "Line Formation"),
	Column	UMETA(DisplayName = "Column Formation"),
	Wedge	UMETA(DisplayName = "Wedge Formation"),
	Blob	UMETA(DisplayName = "Blob Formation")
};

UENUM(BlueprintType)
enum class ECombatBehavior : uint8
{
	Neutral	UMETA(DisplayName = "Neutral Combat Behavior"),
	Passive	UMETA(DisplayName = "Passive Combat Behavior"),
	Aggressive	UMETA(DisplayName = "Aggressive Combat Behavior")
};

UENUM(BlueprintType)
enum class ETeams : uint8
{
	Clone	UMETA(DisplayName = "Clone Team"),
	Droid	UMETA(DisplayName = "Droid Team"),
	HiveMind	UMETA(DisplayName = "HiveMind Team")
};

USTRUCT(BlueprintType)
struct FCommandData
{
	GENERATED_BODY()

	// Initialisation
	FCommandData()
	:	SourceLocation(FVector::ZeroVector),
		Location(FVector::ZeroVector),
		Rotation(FRotator::ZeroRotator),
		Type(ECommandType::CommandMove),
		Target(nullptr) {}

	// Assignation des parametres 
	FCommandData(const FVector InLocation, const FRotator InRotation, const ECommandType InType, AActor* InTarget = nullptr)
	:	SourceLocation(InLocation),
		Location(InLocation),
		Rotation(InRotation),
		Type(InType),
		Target(InTarget) {}


	// Variables
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector SourceLocation;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Location;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator Rotation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<ECommandType> Type;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AActor* Target;
};
