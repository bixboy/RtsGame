#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AiControllerRts.generated.h"

UCLASS()
class RTSGAME_API AAiControllerRts : public AAIController
{
	GENERATED_BODY()

public:
	AAiControllerRts();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
