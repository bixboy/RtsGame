#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Structures/UnitsProduction/UnitProduction.h"
#include "UnitsProductionDataAsset.generated.h"


UCLASS()
class RTSGAME_API UUnitsProductionDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FUnitsProd UnitProduction;
};
