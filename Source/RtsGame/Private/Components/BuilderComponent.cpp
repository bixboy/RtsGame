#include "Components/BuilderComponent.h"


UBuilderComponent::UBuilderComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBuilderComponent::BeginPlay()
{
	Super::BeginPlay();
}

