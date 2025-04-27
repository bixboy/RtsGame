#include "Components/GridComponent.h"
#include "ProceduralMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"


// ---------------- Setup ----------------
#pragma region Setup

UGridComponent::UGridComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f;
	
	LineProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>("LineProceduralMesh");
	SelectionProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>("SelectionProceduralMesh");

	GridCollisionBox = CreateDefaultSubobject<UBoxComponent>("GridCollisionBox");
	if (GridCollisionBox)
	{
		GridCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &UGridComponent::BeginOverlap);
		GridCollisionBox->OnComponentEndOverlap.AddDynamic(this, &UGridComponent::EndOverlap);
	}
}

void UGridComponent::OnRegister()
{
	Super::OnRegister();
	if (GetOwner() && GetOwner()->GetRootComponent())
	{
		LineProceduralMesh->SetupAttachment(GetOwner()->GetRootComponent());
		SelectionProceduralMesh->SetupAttachment(LineProceduralMesh);

		GridCollisionBox->SetupAttachment(LineProceduralMesh);
		SetActiveCollision(bCollisionEnabled);
	}
}

void UGridComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UGridComponent::SetupLines(float Width, float Height)
{
	if (Width > 0.f && Height > 0.f)
    {
        GridWidth = Width;
        GridHeight = Height;    
    }

    float TotalWidth = GetGridWidth();
    float TotalHeight = GetGridHeight();
	
	FVector CenterOffset = FVector(TotalWidth / 2.f, TotalHeight / 2.f, 0.f);

    GridCells.Empty();
    const TArray<FVector> Normals;
    const TArray<FVector2D> UV0;
    const TArray<FColor> VertexColors;
    const TArray<FProcMeshTangent> Tangents;
    
    TArray<FVector> LineVertices;
    TArray<int32> LineTriangles;
    
    UMaterialInstanceDynamic* CellMaterial = CreateMaterial(LineColor, LineOpacity);

    for (int32 Row = 0; Row < GridWidth; ++Row)
    {
        for (int32 Column = 0; Column < GridHeight; ++Column)
        {
            FVector2D CellBL = FVector2D(Row * CellSize, Column * CellSize) - FVector2D(CenterOffset.X, CenterOffset.Y);
            FVector2D CellTR = FVector2D((Row + 1) * CellSize, (Column + 1) * CellSize) - FVector2D(CenterOffset.X, CenterOffset.Y);

            // Horizontale
            {
                FVector Start = FVector(CellBL.X, CellBL.Y, 0.f);
                FVector End   = FVector(CellTR.X, CellBL.Y, 0.f);
                CreateLine(Start, End, LineThickness, LineVertices, LineTriangles);
            }
            // Verticale
            {
                FVector Start = FVector(CellBL.X, CellBL.Y, 0.f);
                FVector End   = FVector(CellBL.X, CellTR.Y, 0.f);
                CreateLine(Start, End, LineThickness, LineVertices, LineTriangles);
            }
            
            FGridCell NewCell;
            NewCell.Position = FVector2D(Row, Column);
            NewCell.MaterialInstance = CellMaterial;
            GridCells.Add(NewCell);
        }
    }
    
    // End Border
	{
	FVector Start;
	FVector End;
	
	float Y = TotalHeight - CenterOffset.Y;
	Start = FVector(-CenterOffset.X, Y, 0.f);
	End = FVector(TotalWidth - CenterOffset.X, Y, 0.f);
	
	CreateLine(Start, End, LineThickness, LineVertices, LineTriangles);
	
	float X = TotalWidth - CenterOffset.X;
	Start = FVector(X, -CenterOffset.Y, 0.f);
	End = FVector(X, TotalHeight - CenterOffset.Y, 0.f);
	
	CreateLine(Start, End, LineThickness, LineVertices, LineTriangles);	
	}

	LineProceduralMesh->ClearAllMeshSections();
    LineProceduralMesh->CreateMeshSection(0, LineVertices, LineTriangles, Normals, UV0, VertexColors, Tangents, false);
    LineProceduralMesh->SetMaterial(0, CellMaterial);
}
// ---------------- Create Line :

void UGridComponent::CreateLine(FVector Start, FVector End, float Thickness, TArray<FVector>& Vertices, TArray<int>& Triangles)
{
	FVector ThicknessDirection = End - Start;
	
	UKismetMathLibrary::Vector_Normalize(ThicknessDirection, 0.0001f);
	ThicknessDirection = UKismetMathLibrary::Cross_VectorVector(ThicknessDirection, FVector(0.f, 0.f, 1.f));
	
	float HalfThickness = Thickness / 2.f;

	TArray<int32> IntList;
	IntList.Add(Vertices.Num() + 2);
	IntList.Add(Vertices.Num() + 1);
	IntList.Add(Vertices.Num() + 0);
	
	IntList.Add(Vertices.Num() + 2);
	IntList.Add(Vertices.Num() + 3);
	IntList.Add(Vertices.Num() + 1);

	Triangles.Append(IntList);

	TArray<FVector> VectorList;
	VectorList.Add(Start + (ThicknessDirection * HalfThickness));
	VectorList.Add(End + (ThicknessDirection * HalfThickness));

	VectorList.Add(Start - (ThicknessDirection * HalfThickness));
	VectorList.Add(End - (ThicknessDirection * HalfThickness));

	Vertices.Append(VectorList);
}

UMaterialInstanceDynamic* UGridComponent::CreateMaterial(FLinearColor Color, float Opacity)
{
	if (UMaterialInstanceDynamic* MaterialInstance = UMaterialInstanceDynamic::Create(Material, this))
	{
		MaterialInstance->SetVectorParameterValue("Color", Color);
		MaterialInstance->SetScalarParameterValue("Opacity", Opacity);

		return MaterialInstance;
	}

	return nullptr;
}

// ---------------- Getter :

float UGridComponent::GetGridWidth()
{
	return GridWidth * CellSize;
}

float UGridComponent::GetGridHeight()
{
	return GridHeight * CellSize;
}

float UGridComponent::GetCellSize()
{
	return CellSize;
}

#pragma endregion


// ---------------- Selection ----------------
#pragma region Selection

bool UGridComponent::GetCellsIsOccupied(bool bAllCells)
{
	if (bAllCells)
	{
		// Vérifier que toutes les cellules sont occupées
		for (const FGridCell& Cell : GridCells)
		{
			if (!Cell.bOccupied)
			{
				return false;
			}
		}
		return true;
	}
	else
	{
		// Vérifier qu'au moins une cellule est occupée
		for (const FGridCell& Cell : GridCells)
		{
			if (Cell.bOccupied)
			{
				return true;
			}
		}
		return false;
	}
}

void UGridComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (bCollisionEnabled && bActorInGrid)
	{
		CheckOverlappingCells();	
	}
}

void UGridComponent::CheckOverlappingCells()
{
    if (GridCells.IsEmpty() || !bCollisionEnabled) return;

    TArray<FVector2D> CellsToDisplay;

    // Parcourir tous les acteurs en overlap
    for (AActor* OverlappingActor : OverlappingActors)
    {
        if (OverlappingActor)
        {
            TArray<FVector2D> ActorTiles = GetTilesForActor(OverlappingActor);
            for (const FVector2D& Tile : ActorTiles)
            {
                bool bAlreadyPresent = false;
                for (const FVector2D& ExistingTile : CellsToDisplay)
                {
                    if (ExistingTile.Equals(Tile, 0.01f))
                    {
                        bAlreadyPresent = true;
                        break;
                    }
                }
                if (!bAlreadyPresent)
                {
                    CellsToDisplay.Add(Tile);
                }
            }
        }
    }

    // Mise à jour de l'occupation pour chaque cellule
	for (FGridCell& Cell : GridCells)
	{
		FVector2D CellId(Cell.Position.X, Cell.Position.Y);
		bool bFound = CellsToDisplay.ContainsByPredicate([&](const FVector2D& Other)
		{
			return Other.Equals(CellId, 0.001f);
		});
		
		Cell.bOccupied = bFound;
	}

    if (CellsToDisplay.Num() > 0)
    {
        CreateSelectionCells(CellsToDisplay);
    }
    else
    {
        SelectionProceduralMesh->ClearAllMeshSections();
    }
}

bool UGridComponent::GetCollisionIsEnabled()
{
	return bCollisionEnabled;
}

void UGridComponent::EnabledCollision(bool NewEnable)
{
	bCollisionEnabled = NewEnable;

	LineProceduralMesh->SetVisibility(bCollisionEnabled);
	
	SelectionProceduralMesh->SetVisibility(bCollisionEnabled);
	SelectionProceduralMesh->ClearAllMeshSections();
}

TArray<FVector2D> UGridComponent::GetTilesForActor(AActor* Actor)
{
	if (!Actor) return TArray<FVector2D>();
	
	FBox MeshBox;
	TArray<FVector2D> Tiles;

	if (USkeletalMeshComponent* SkeletalComp = Actor->FindComponentByClass<USkeletalMeshComponent>())
	{
		MeshBox = SkeletalComp->Bounds.GetBox();
	}
	else
	{
		if (UStaticMeshComponent* MeshComp = Actor->FindComponentByClass<UStaticMeshComponent>())
			MeshBox = MeshComp->Bounds.GetBox();
	}
    
	float TotalWidth  = GetGridWidth();
	float TotalHeight = GetGridHeight();
	
	FVector CenterOffset = FVector(TotalWidth / 2.f, TotalHeight / 2.f, 0.f);
	FVector GridOrigin = GetOwner()->GetActorLocation() - CenterOffset;
    
	float LocalMinX = MeshBox.Min.X - GridOrigin.X;
	float LocalMinY = MeshBox.Min.Y - GridOrigin.Y;
	float LocalMaxX = MeshBox.Max.X - GridOrigin.X;
	float LocalMaxY = MeshBox.Max.Y - GridOrigin.Y;
    
	int MinRow = FMath::FloorToInt(LocalMinX / CellSize);
	int MaxRow = FMath::FloorToInt(LocalMaxX / CellSize);
	int MinCol = FMath::FloorToInt(LocalMinY / CellSize);
	int MaxCol = FMath::FloorToInt(LocalMaxY / CellSize);
    
	for (int r = MinRow; r <= MaxRow; r++)
	{
		for (int c = MinCol; c <= MaxCol; c++)
		{
			if (GetTileIsValid(r, c))
			{
				Tiles.Add(FVector2D(r, c));
			}
		}
	}
    
	return Tiles;
}

void UGridComponent::CreateSelectionCells(TArray<FVector2D> CellsToDisplay)
{
    SelectionProceduralMesh->ClearAllMeshSections();

    TArray<FVector> AllVertices;
    TArray<int32> AllTriangles;

    for (const FVector2D CellLocation : CellsToDisplay)
    {
        int32 Row = CellLocation.X;
        int32 Column = CellLocation.Y;

        if (GetTileIsValid(Row, Column))
        {
            FVector2D CenterOffset = FVector2D(GetGridWidth() / 2.f, GetGridHeight() / 2.f);
            FVector WorldLocation(Row * CellSize - CenterOffset.X, Column * CellSize - CenterOffset.Y, 0.f);

            TArray<FVector> Vertices;
            TArray<int32> Triangles;
            CreateLine(FVector(0.f, CellSize / 2.f, 0.f), FVector(CellSize, CellSize / 2.f, 0.f), CellSize, Vertices, Triangles);

            for (FVector& Vertex : Vertices)
            {
                Vertex += WorldLocation;
            }
            int32 VertexOffset = AllVertices.Num();
        	AllVertices.Append(Vertices);
        	
            for (int32& TriangleIndex : Triangles)
            {
                TriangleIndex += VertexOffset;
            }

            AllTriangles.Append(Triangles);
        }
    }

    TArray<FVector> Normals;
    TArray<FVector2D> UV0;
    TArray<FColor> VertexColors;
    TArray<FProcMeshTangent> Tangents;
    SelectionProceduralMesh->CreateMeshSection(0, AllVertices, AllTriangles, Normals, UV0, VertexColors, Tangents, false);

    UMaterialInstanceDynamic* CellMaterial = CreateMaterial(SelectionColor, SelectionOpacity);
    SelectionProceduralMesh->SetMaterial(0, CellMaterial);
}

void UGridComponent::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bCollisionEnabled) return;
	
	if (OtherActor && (OtherActor != GetOwner()) && OtherComp)
	{
		bActorInGrid = true;
		if (!OverlappingActors.Contains(OtherActor))
		{
			OverlappingActors.Add(OtherActor);
		}
	}
}

void UGridComponent::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!bCollisionEnabled) return;
	
	if (OtherActor && (OtherActor != GetOwner()) && OtherComp)
	{
		OverlappingActors.Remove(OtherActor);
		if (OverlappingActors.Num() == 0)
		{
			CheckOverlappingCells();
			bActorInGrid = false;
		}
	}
}

#pragma endregion


// ---------------- Convertion ----------------
#pragma region Convertion Functions

bool UGridComponent::GetLocationToTile(FVector Location, int& Row, int& Column)
{
    float TotalWidth  = GetGridWidth();
    float TotalHeight = GetGridHeight();

    FVector CenterOffset = FVector(TotalWidth / 2.f, TotalHeight / 2.f, 0.f);
    FVector GridOrigin = GetOwner()->GetActorLocation() - CenterOffset;

    float LocalX = Location.X - GridOrigin.X;
    float LocalY = Location.Y - GridOrigin.Y;

    Row    = FMath::FloorToInt(LocalX / CellSize);
    Column = FMath::FloorToInt(LocalY / CellSize);

    return GetTileIsValid(Row, Column);
}

bool UGridComponent::GetTileToGrid(int Row, int Column, bool bCenter, FVector2D& GridLocation)
{
	float X = Column * CellSize + GetOwner()->GetActorLocation().X;
	float Y = Row * CellSize + GetOwner()->GetActorLocation().Y;
	if (bCenter)
	{
		X += CellSize / 2.f;
		Y += CellSize / 2.f;
	}
	GridLocation = FVector2D(X, Y);
	return GetTileIsValid(Row, Column);
}

bool UGridComponent::GetTileIsValid(int Row, int Column)
{
	return (Row >= 0 && Row < GridHeight && Column >= 0 && Column < GridWidth);
}

void UGridComponent::InvertColorCells(bool bReverseIfAlreadyReversed)
{
	if (bReverseIfAlreadyReversed)
	{
		if (!bColorInverted) return;
	}
	
	FLinearColor Cells = LineColor;

	LineColor = SelectionColor;
	SelectionColor = Cells;
		
	bColorInverted = !bColorInverted;
}

void UGridComponent::SetActiveCollision(bool bNewActive)
{
	bCollisionEnabled = bNewActive;
	if (bCollisionEnabled)
	{
		FVector BoxExtent = FVector(GridWidth * CellSize / 2.f, GridHeight * CellSize / 2.f, 50.f);
		
		GridCollisionBox->SetBoxExtent(BoxExtent);
		GridCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		GridCollisionBox->SetCollisionResponseToAllChannels(ECR_Overlap);
		GridCollisionBox->SetGenerateOverlapEvents(true);
	}
	else
	{
		GridCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GridCollisionBox->SetBoxExtent(FVector(32.f, 32.f, 32.f));
	}
}

#pragma endregion
