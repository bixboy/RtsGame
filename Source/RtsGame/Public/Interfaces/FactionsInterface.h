#pragma once
#include "CoreMinimal.h"
#include "Data/DataRts.h"
#include "UObject/Interface.h"
#include "FactionsInterface.generated.h"


UINTERFACE()
class UFactionsInterface : public UInterface
{
	GENERATED_BODY()
};

class RTSGAME_API IFactionsInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	EFaction GetCurrentFaction();
};
