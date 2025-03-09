#pragma once

#include "CoreMinimal.h"
#include "Framwork/SLobbyPlayerController.h"
#include "UObject/Object.h"
#include "SPlayerData.generated.h"


UCLASS()
class MULTIPLAYERMENU_API USPlayerData : public UObject
{
	GENERATED_BODY()

public:
	FPlayerInfo PlayerInfo;
};
