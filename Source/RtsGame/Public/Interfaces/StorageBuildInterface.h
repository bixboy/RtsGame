#pragma once
#include "CoreMinimal.h"
#include "Data/DataRts.h"
#include "UObject/Interface.h"
#include "StorageBuildInterface.generated.h"


UINTERFACE()
class UStorageBuildInterface : public UInterface
{
	GENERATED_BODY()
};


class RTSGAME_API IStorageBuildInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FResourcesCost GetResource();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FResourcesCost GetMaxResource();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetResourceByType(EResourceType ResourceType = EResourceType::None);


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetMaxByTypeResource(EResourceType ResourceType = EResourceType::None);
};
