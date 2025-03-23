#include "Structures/StructurePreview.h"
#include "Components/GridComponent.h"
#include "Manager/GridManager.h"
#include "Structures/StructureBase.h"


AStructurePreview::AStructurePreview()
{
	PrimaryActorTick.bCanEverTick = true;

	GridComponent = CreateDefaultSubobject<UGridComponent>("Grid Component");

	BuildingWidth = 1;
	BuildingHeight = 1;
}

void AStructurePreview::BeginPlay()
{
	Super::BeginPlay();
}

void AStructurePreview::CheckIsValidPlacement()
{
	if (GetIsValidPlacement())
	{
		if (MaterialInstance)
			MaterialInstance->SetVectorParameterValue("Color", FLinearColor::Green);
	}
	else
	{
		if (MaterialInstance)
			MaterialInstance->SetVectorParameterValue("Color",FLinearColor::Red);
	}
}

bool AStructurePreview::GetIsValidPlacement()
{
	if (GridComponent)
	{
		if (GridComponent->GetCollisionIsEnabled())
		{
			if (!bAreaPlacementValid)
			{
				return !GridComponent->GetCellsIsOccupied();
			}
			
			return GridComponent->GetCellsIsOccupied(true);
		}
		
		return Super::GetIsValidPlacement();
	}
	
	return false;
}

void AStructurePreview::StartPlacingBuilding(TSubclassOf<AStructureBase> BuildingClass)
{
	if (!BuildingClass) return;

    if (AStructureBase* DefaultBuild = BuildingClass->GetDefaultObject<AStructureBase>())
    {
        Mesh = DefaultBuild->GetStaticMesh();
        MeshScale = DefaultBuild->GetActorRelativeScale3D();
    	
        ShowPreview(Mesh, MeshScale);
    }
	
	if (!bShowGrid) return;
	
    if (StaticMesh && GridComponent)
    {
    	FVector2D Size = ComputeGridSize(GridComponent->GetCellSize());

	    if (bAreaPlacementValid)
	    {
	    	GridComponent->InvertColorCells();   
	    }
		else
		{
			GridComponent->InvertColorCells(true);
		}
    	
    	GridComponent->SetupLines(Size.X, Size.Y);
    	GridComponent->SetActiveCollision(true);
    }
}

FVector2D AStructurePreview::ComputeGridSize(float CellSize)
{
	if (!StaticMesh || !Mesh || CellSize <= 0.f)
	{
		BuildingWidth = 1;
		BuildingHeight = 1;
		return FVector2D();
	}

	// Obtenir les dimensions du maillage
	FBoxSphereBounds MeshBounds = Mesh->GetBounds();
	FVector ScaledExtent = MeshBounds.BoxExtent * MeshScale;

	// Obtenir la rotation du maillage
	FQuat MeshRotation = StaticMesh->GetComponentQuat();

	// Définir les coins du maillage en coordonnées locales
	FVector LocalCorners[4] = {
		FVector(-ScaledExtent.X, -ScaledExtent.Y, 0.f),
		FVector(ScaledExtent.X, -ScaledExtent.Y, 0.f),
		FVector(ScaledExtent.X, ScaledExtent.Y, 0.f),
		FVector(-ScaledExtent.X, ScaledExtent.Y, 0.f)
	};

	// Initialiser les valeurs minimales et maximales projetées
	float MinX = FLT_MAX, MaxX = -FLT_MAX;
	float MinY = FLT_MAX, MaxY = -FLT_MAX;

	// Calculer les coins projetés en fonction de la rotation
	for (const FVector& Corner : LocalCorners)
	{
		FVector RotatedCorner = MeshRotation.RotateVector(Corner);
		MinX = FMath::Min(MinX, RotatedCorner.X);
		MaxX = FMath::Max(MaxX, RotatedCorner.X);
		MinY = FMath::Min(MinY, RotatedCorner.Y);
		MaxY = FMath::Max(MaxY, RotatedCorner.Y);
	}

	// Calculer la largeur et la hauteur projetées
	float ProjectedWidth = MaxX - MinX;
	float ProjectedHeight = MaxY - MinY;

	// Calculer le nombre de cellules nécessaires
	BuildingWidth = FMath::CeilToInt(ProjectedWidth / CellSize);
	BuildingHeight = FMath::CeilToInt(ProjectedHeight / CellSize);

	// S'assurer que la grille a au moins une cellule
	BuildingWidth = FMath::Max(BuildingWidth, 1);
	BuildingHeight = FMath::Max(BuildingHeight, 1);

	return FVector2D(BuildingWidth, BuildingHeight);
}

void AStructurePreview::EnabledCollision(bool NewEnable)
{
	GridComponent->EnabledCollision(NewEnable);
}


