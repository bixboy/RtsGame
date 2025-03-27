#pragma once
#include "CoreMinimal.h"
#include "Data/DataRts.h"
#include "UObject/Interface.h"
#include "UnitTypeInterface.generated.h"


UINTERFACE()
class UUnitTypeInterface : public UInterface
{
	GENERATED_BODY()
};


class RTSGAME_API IUnitTypeInterface
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	EUnitsType GetUnitType();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void MoveToBuild(AStructureBase* BuildDest);
};
