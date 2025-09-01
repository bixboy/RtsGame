#pragma once
#include "CoreMinimal.h"
#include "Data/DataRts.h"
#include "Data/WorkerData.h"
#include "UObject/Interface.h"
#include "UnitTypeInterface.generated.h"

class AResourceDepot;
class AResourceNode;


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
	void StartWork(const FTaskJob& Job);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void MoveToResourceNode(AResourceNode* Node);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void MoveToResourceStorage(AResourceDepot* Storage);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UUnitsProductionDataAsset* GetUnitData();
};
