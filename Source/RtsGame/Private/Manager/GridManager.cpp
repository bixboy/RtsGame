#include "Manager/GridManager.h"
#include "Components/GridComponent.h"


AGridManager::AGridManager()
{
	PrimaryActorTick.bCanEverTick = true;

	GridComponent = CreateDefaultSubobject<UGridComponent>(TEXT("Grid Component"));
}

void AGridManager::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	GridComponent->SetupLines();

	TArray<FVector2D> Points;
	Points.Add(FVector2D::ZeroVector);
	Points.Add(FVector2D(9, 9));
	Points.Add(FVector2D(3, 4));
	
	GridComponent->CreateSelectionCells(Points);
}

void AGridManager::BeginPlay()
{
	Super::BeginPlay();
}

