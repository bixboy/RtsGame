#pragma once
#include "CoreMinimal.h"
#include "Units/SoldierRts.h"
#include "DataRts.generated.h"

class AStructureBase;
class UStructureDataAsset;


UENUM(BlueprintType)
enum class EStructureType : uint8
{
	Structure,
	Defence,
	None
};

UENUM(BlueprintType)
enum class EFaction : uint8
{
	DwarfExplorer,
	DwarfAggressive,
	None
};

// ------- Resources Cost -------
USTRUCT(BlueprintType)
struct FResourcesCost
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
	int Woods = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
	int Food = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
	int Metal = 0;
	
	FResourcesCost operator+(const FResourcesCost& Other) const
	{
		FResourcesCost Sum;
		Sum.Woods = Woods + Other.Woods;
		Sum.Food = Food + Other.Food;
		Sum.Metal = Metal + Other.Metal;
		return Sum;
	}

	FResourcesCost operator-(const FResourcesCost& Other) const
	{
		FResourcesCost Diff;
		Diff.Woods = Woods - Other.Woods;
		Diff.Food = Food - Other.Food;
		Diff.Metal = Metal - Other.Metal;
		return Diff;
	}


	bool operator==(const FResourcesCost& Other) const
	{
		return	Woods == Other.Woods &&
				Food == Other.Food &&
				Metal == Other.Metal;
	}
};

// ------- Builds Upgrade -------
USTRUCT(BlueprintType)
struct FStructureUpgrade
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrades")
	EFaction FactionUpgrade = EFaction::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FResourcesCost UpgradeCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	UStructureDataAsset* NextStructure;

	bool operator==(const FStructureUpgrade& Other) const
	{
		return	FactionUpgrade == Other.FactionUpgrade &&
				UpgradeCost == Other.UpgradeCost &&
				NextStructure == Other.NextStructure;
	}
};

// ------- Builds Data -------
USTRUCT(BlueprintType)
struct FStructure
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FString Name = "Name";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FString Description = "Description";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	EStructureType StructureType = EStructureType::Structure;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	UTexture2D* Image = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	int Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Build")
	TSubclassOf<AStructureBase> BuildClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Build")
	UStaticMesh* StructureMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Build")
	TArray<UStaticMesh*> BuildSteps;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Build")
	FResourcesCost BuildCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Build")
	float TimeToBuild = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Build")
	TArray<FStructureUpgrade> Upgrades;


	bool operator==(const FStructure& Other) const
	{
		return	Name == Other.Name &&
				Description == Other.Description &&
				StructureType == Other.StructureType &&
				Id == Other.Id &&
				StructureMesh == Other.StructureMesh &&
				BuildSteps == Other.BuildSteps &&
				TimeToBuild == Other.TimeToBuild &&
				Upgrades == Other.Upgrades;
	}
};


// ------- Units Prod -------
USTRUCT(BlueprintType)
struct FUnitsProd
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit")
	FString Name = "Name";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit")
	FString Description = "Description";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit")
	UTexture2D* UnitImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit")
	TSubclassOf<ASoldierRts> UnitClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Unit|Production")
	FResourcesCost ProductionCost;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Unit|Production")
	float ProductionTime = 5.f;

	bool operator==(const FUnitsProd& Other) const
	{
		return	Name == Other.Name &&
				Description == Other.Description &&
				UnitImage == Other.UnitImage &&
				UnitClass == Other.UnitClass &&
				UnitImage == Other.UnitImage &&
				ProductionCost == Other.ProductionCost &&
				ProductionTime == Other.ProductionTime;
	}
};
