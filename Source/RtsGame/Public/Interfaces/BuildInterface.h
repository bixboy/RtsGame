#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BuildInterface.generated.h"

class UStructureDataAsset;


UINTERFACE()
class UBuildInterface : public UInterface
{
	GENERATED_BODY()
};

class RTSGAME_API IBuildInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UStructureDataAsset* GetDataAsset();
};
