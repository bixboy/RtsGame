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

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	TArray<UUnitsProductionDataAsset*> GetUnitsProduction();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	TArray<UUnitsProductionDataAsset*> GetUnitsInQueueByClass(TSubclassOf<AActor> FilterClass);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	TArray<UUnitsProductionDataAsset*> GetProductionList();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	float GetProductionProgress();
	
};
