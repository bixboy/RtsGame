#pragma once

#include "CoreMinimal.h"
#include "CommonGameInstance.h"
#include "SGameInstance.generated.h"

UCLASS()
class MULTIPLAYERMENU_API USGameInstance : public UCommonGameInstance
{
	GENERATED_BODY()

public :
	USGameInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
