#include "Structures/Wall/WallSegment.h"


AWallSegment::AWallSegment()
{
	PrimaryActorTick.bCanEverTick = false;

	SnapPointFront = CreateDefaultSubobject<USceneComponent>(TEXT("SnapFront"));
	SnapPointFront->SetupAttachment(RootComponent);
	SnapPointFront->SetRelativeLocation(FVector(100.f, 0, 0));

	SnapPointBack = CreateDefaultSubobject<USceneComponent>(TEXT("SnapBack"));
	SnapPointBack->SetupAttachment(RootComponent);
	SnapPointBack->SetRelativeLocation(FVector(-100.f, 0, 0));
}

void AWallSegment::BeginPlay()
{
	Super::BeginPlay();
}

