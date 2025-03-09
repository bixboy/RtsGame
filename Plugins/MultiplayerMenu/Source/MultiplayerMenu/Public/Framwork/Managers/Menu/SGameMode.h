#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "SGameMode.generated.h"

class ASPlayerController;

UCLASS()
class MULTIPLAYERMENU_API ASGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ASGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void InitGameState() override;

protected:
	UPROPERTY()
	TArray<ASPlayerController*> SessionPlayers;
};
