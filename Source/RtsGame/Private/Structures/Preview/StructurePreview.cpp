#include "Structures/Preview/StructurePreview.h"
#include "Components/GridComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/SlectionComponent.h"
#include "Engine/OverlapResult.h"
#include "Player/PlayerControllerRts.h"
#include "Player/RtsPlayer.h"
#include "Structures/StructureBase.h"


// ========= Setup ========= //
#pragma region Setup

AStructurePreview::AStructurePreview()
{
	PrimaryActorTick.bCanEverTick = true;

	GridComponent = CreateDefaultSubobject<UGridComponent>("Grid Component");
	
	USceneComponent* InstanceFill = CreateDefaultSubobject<USceneComponent>("MeshInstance");
	
	// valid
	PreviewValidInstance = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("ValidInstance"));
	PreviewValidInstance->SetupAttachment(InstanceFill);
	PreviewValidInstance->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PreviewValidInstance->SetCastShadow(false);

	// invalid
	PreviewInvalidInstance = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InvalidInstance"));
	PreviewInvalidInstance->SetupAttachment(InstanceFill);
	PreviewInvalidInstance->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PreviewInvalidInstance->SetCastShadow(false);

	BuildingWidth = 1;
	BuildingHeight = 1;
}

void AStructurePreview::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<ARtsPlayer>(GetOwner());
}

#pragma endregion


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

void AStructurePreview::StartPlacingBuilding(const FStructure BuildData)
{
	if (!BuildData.StructureMesh) return;
	
    Mesh = BuildData.StructureMesh;
    MeshScale = FVector(1.f, 1.f, 1.f);

	bIsPreviewingWall = false;
    
    ShowPreview(Mesh, MeshScale);
	
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


// ========= Wall Preview ========= //
void AStructurePreview::StartWallPreview(const FStructure BuildData)
{
	StartPlacingBuilding(BuildData);

	PreviewValidInstance->SetVisibility(false);
	PreviewInvalidInstance->SetVisibility(false);
	
	CachedMeshScale = MeshScale;
	Mesh = BuildData.StructureMesh;

	if (BuildData.BuildClass)
	{
		if (AStructureBase* CDO = BuildData.BuildClass->GetDefaultObject<AStructureBase>())
		{
			UStaticMeshComponent* Comp = CDO->GetMeshComponent();
			if (Comp && Comp->GetStaticMesh())
			{
				float LocalLen = Comp->GetStaticMesh()->GetBounds().BoxExtent.X * 2.f;
				CachedSegmentLength = LocalLen * CachedMeshScale.X;
				
				PreviewValidInstance->SetStaticMesh(Mesh);
				PreviewInvalidInstance->SetStaticMesh(Mesh);
				
				if (HighlightMaterial)
				{
					UMaterialInstanceDynamic* ValidationMaterial = UMaterialInstanceDynamic::Create(HighlightMaterial, this);
					PreviewValidInstance->SetOverlayMaterial(ValidationMaterial);

					UMaterialInstanceDynamic* InvalidMaterial = UMaterialInstanceDynamic::Create(HighlightMaterial, this);
					InvalidMaterial->SetScalarParameterValue("Status", 0.f);
					
					PreviewInvalidInstance->SetOverlayMaterial(InvalidMaterial);
				}
			}
		}
	}
}

void AStructurePreview::ShowWallPreview(FVector MouseLocation)
{
	StartTransform = MouseLocation;
	bIsPreviewingWall = true;
	
	PreviewValidInstance->SetVisibility(true);
	PreviewInvalidInstance->SetVisibility(true);
}

void AStructurePreview::StopWallPreview()
{
	StartTransform = FVector::ZeroVector;
	bIsPreviewingWall = false;
	
	PreviewValidInstance->SetVisibility(false);
	PreviewInvalidInstance->SetVisibility(false);
}

void AStructurePreview::Tick(float Delta)
{
	Super::Tick(Delta);

	if (!bIsPreviewingWall || CachedSegmentLength <= KINDA_SMALL_NUMBER)
		return;

	PreviewValidInstance->ClearInstances();
	PreviewInvalidInstance->ClearInstances();

	const FVector Start = StartTransform;
	const FVector Current = Player->GetRtsPlayerController()->SelectionComponent->GetMousePositionOnTerrain().Location;

	const FVector Dir = (Current - Start).GetSafeNormal();
	const float TotalDist = FVector::Dist(Start, Current);
	
	if (TotalDist <= KINDA_SMALL_NUMBER) 
		return;

	const float SegLen = CachedSegmentLength;
	const int32 Count = FMath::FloorToInt(TotalDist / SegLen);
	const FRotator Rot = Dir.Rotation();

	const float HalfOffset  = SegLen * 0.5f;

	FVector BoxExtent(0);
	if (Mesh)
	{
		const FVector LocalExtent = Mesh->GetBounds().BoxExtent;
		BoxExtent = LocalExtent * MeshScale; 
	}

	bool bStillValid = true;
	for (int32 i = 0; i < Count; ++i)
	{
		const float DistAlong = SegLen * i + HalfOffset;
		const FVector InstanceLoc = Start + Dir * DistAlong;
		
		TArray<FOverlapResult> Hits;
		FCollisionQueryParams QP;
		QP.AddIgnoredActor(this);

		if (GetWorld()->OverlapAnyTestByChannel(InstanceLoc, Rot.Quaternion(), ECC_GameTraceChannel2, FCollisionShape::MakeBox(BoxExtent), QP))
		{
			bStillValid = false;
		}

		const FTransform Tf(Rot, InstanceLoc, CachedMeshScale);
		if (bStillValid)
		{
			PreviewValidInstance->AddInstance(Tf, true);	
		}
		else
		{
			PreviewInvalidInstance->AddInstance(Tf, true);	
		}
		
	}
}

TArray<FTransform> AStructurePreview::ConfirmWallPreview()
{
	TArray<FTransform> SpawnsLocation;
	if (!bIsPreviewingWall) return SpawnsLocation;

	bIsPreviewingWall = false;

	int32 Num = PreviewValidInstance->GetInstanceCount();
	for (int32 i = 0; i < Num; ++i)
	{
		FTransform Tf;
		PreviewValidInstance->GetInstanceTransform(i, Tf, true);
		
		SpawnsLocation.Add(Tf);
	}

	return SpawnsLocation;
}


// ========= Grid ========= //
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


