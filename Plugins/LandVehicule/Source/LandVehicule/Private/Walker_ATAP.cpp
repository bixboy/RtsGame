#include "Walker_ATAP.h"

AWalker_ATAP::AWalker_ATAP()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWalker_ATAP::BeginPlay()
{
	Super::BeginPlay();
}

void AWalker_ATAP::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

