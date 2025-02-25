#pragma once

#include "CoreMinimal.h"
#include "FindSessionsCallbackProxy.h"
#include "UObject/Object.h"
#include "BlueprintSessionResultObject.generated.h"

UCLASS()
class MULTIPLAYERMENU_API UBlueprintSessionResultObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Session")
	FBlueprintSessionResult SessionResult;
};
