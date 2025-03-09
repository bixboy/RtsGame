#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SLobbyGameState.generated.h"

class ASLobbyPlayerState;
class USPlayerLobbyWidget;
struct FPlayerInfo;


UCLASS()
class MULTIPLAYERMENU_API ASLobbyGameState : public AGameState
{
	GENERATED_BODY()

public:
	ASLobbyGameState(const FObjectInitializer& ObjectInitializer);
	
};
