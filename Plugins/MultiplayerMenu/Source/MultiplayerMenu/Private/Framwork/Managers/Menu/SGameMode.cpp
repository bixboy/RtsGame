#include "Framwork/Managers/Menu/SGameMode.h"
#include "Framwork/Managers/Menu/SGameState.h"

ASGameMode::ASGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void ASGameMode::InitGameState()
{
	Super::InitGameState();
	SessionPlayers.Empty();
}
