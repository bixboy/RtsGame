#include "RtsGame/Public/AiControllerRts.h"

AAiControllerRts::AAiControllerRts()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAiControllerRts::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAiControllerRts::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

