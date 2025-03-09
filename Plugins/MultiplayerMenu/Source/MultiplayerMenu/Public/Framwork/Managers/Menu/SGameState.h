#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SGameState.generated.h"

UCLASS()
class MULTIPLAYERMENU_API ASGameState : public AGameState
{
	GENERATED_BODY()

public:
	ASGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
};
