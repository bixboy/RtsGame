#pragma once
#include "CoreMinimal.h"
#include "Player/PlayerControllerRts.h"
#include "RtsPlayerController.generated.h"

class URtsComponent;


UCLASS()
class RTSGAME_API ARtsPlayerController : public APlayerControllerRts
{
	GENERATED_BODY()
	
public:
	ARtsPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY()
	URtsComponent* RtsComponent;
};
