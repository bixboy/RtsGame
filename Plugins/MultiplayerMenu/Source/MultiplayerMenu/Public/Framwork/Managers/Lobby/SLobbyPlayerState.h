#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SLobbyPlayerState.generated.h"


UCLASS()
class MULTIPLAYERMENU_API ASLobbyPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ASLobbyPlayerState(const FObjectInitializer& ObjectInitializer);
};
