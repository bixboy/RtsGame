#include "Components/MapIconComponent.h"
#include "EngineUtils.h"
#include "Manager/DynamicCameraMap.h"


UMapIconComponent::UMapIconComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMapIconComponent::BeginPlay()
{
	Super::BeginPlay();

	for (TActorIterator<ADynamicCameraMap> It(GetWorld()); It; ++It)
	{
		CachedMapCam = *It;
		break;
	}
	if (CachedMapCam)
	{
		CachedMapCam->RegisterMapIcon(this);
	}
}


void UMapIconComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (CachedMapCam)
	{
		CachedMapCam->UnregisterMapIcon(this);
	}
	Super::EndPlay(EndPlayReason);
}

