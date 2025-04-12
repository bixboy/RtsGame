#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UnitProductionInterface.generated.h"

class UUnitsProductionDataAsset;


UINTERFACE()
class UUnitProductionInterface : public UInterface
{
	GENERATED_BODY()
};

class RTSGAME_API IUnitProductionInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AddUnitToProduction(UUnitsProductionDataAsset* NewUnit);
};
