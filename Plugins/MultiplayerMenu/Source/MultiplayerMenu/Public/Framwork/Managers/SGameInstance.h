#pragma once

#include "CoreMinimal.h"
#include "AdvancedFriendsGameInstance.h"
#include "SGameInstance.generated.h"

UCLASS()
class MULTIPLAYERMENU_API USGameInstance : public UAdvancedFriendsGameInstance
{
	GENERATED_BODY()

public :
	USGameInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
