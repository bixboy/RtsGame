#include "WorldGeneration/WorldGeneration.h"
#include "ProceduralMeshComponent.h"
#include "Math/UnrealMathUtility.h"

// -------------------- Setup --------------------
#pragma region Setup

AWorldGeneration::AWorldGeneration()
{
	PrimaryActorTick.bCanEverTick = true;

	ProcMeshComp = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProcMesh"));
	RootComponent = ProcMeshComp;
	ProcMeshComp->bUseAsyncCooking = true;
	
	// Valeurs par défaut
	MapWidth = 100;
	MapHeight = 100;
	NumberOfCavities = 4;
	CavitySizeFactor = 1.0f;
	CavityElongation = 2.f;
}

void AWorldGeneration::BeginPlay()
{
	Super::BeginPlay();
	//GenerateMap();
}

void AWorldGeneration::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	GenerateMap();
}

#pragma endregion

void AWorldGeneration::GenerateMap()
{
    Grid.Empty();
    Grid.SetNum(MapWidth);
    for (int32 x = 0; x < MapWidth; ++x)
    {
        Grid[x].SetNum(MapHeight);
        for (int32 y = 0; y < MapHeight; ++y)
        {
            Grid[x][y] = true;
        }
    }
    
    CarveCavities();
    
    const int32 Iterations = 2;
    for (int32 iteration = 0; iteration < Iterations; ++iteration)
    {
        TArray<TArray<bool>> NewGrid = Grid;
        for (int32 x = 0; x < MapWidth; ++x)
        {
            for (int32 y = 0; y < MapHeight; ++y)
            {
                if (!Grid[x][y])
                {
                    NewGrid[x][y] = false;
                    continue;
                }
                
                int32 SolidCount = 0;
                for (int32 dx = -1; dx <= 1; ++dx)
                {
                    for (int32 dy = -1; dy <= 1; ++dy)
                    {
                        if (dx == 0 && dy == 0)
                            continue;
                        int32 nx = x + dx;
                        int32 ny = y + dy;
                        if (nx < 0 || ny < 0 || nx >= MapWidth || ny >= MapHeight)
                        {
                            SolidCount++;
                        }
                        else if (Grid[nx][ny])
                        {
                            SolidCount++;
                        }
                    }
                }
                NewGrid[x][y] = (SolidCount >= 5);
            }
        }
        Grid = NewGrid;
    }
    
    GenerateSpawnPoints();

    GenerateMesh();
}

void AWorldGeneration::GenerateMesh()
{
    TArray<FVector> Vertices;
    TArray<int32> Triangles;
    TArray<FVector> Normals;
    TArray<FVector2D> UV0;
    TArray<FProcMeshTangent> Tangents;
    TArray<FColor> VertexColors;

    for (int32 x = 0; x < MapWidth; ++x)
    {
        for (int32 y = 0; y < MapHeight; ++y)
        {
            if (Grid[x][y])
            {
                FVector BottomLeft = FVector(x * CellSize, y * CellSize, 0);
                FVector BottomRight = FVector((x + 1) * CellSize, y * CellSize, 0);
                FVector TopRight = FVector((x + 1) * CellSize, (y + 1) * CellSize, 0);
                FVector TopLeft = FVector(x * CellSize, (y + 1) * CellSize, 0);

                int32 VertexStart = Vertices.Num();
                Vertices.Add(BottomLeft);
                Vertices.Add(BottomRight);
                Vertices.Add(TopRight);
                Vertices.Add(TopLeft);

                Triangles.Add(VertexStart + 0);
                Triangles.Add(VertexStart + 1);
                Triangles.Add(VertexStart + 2);

                Triangles.Add(VertexStart + 0);
                Triangles.Add(VertexStart + 2);
                Triangles.Add(VertexStart + 3);

                Normals.Add(FVector::UpVector);
                Normals.Add(FVector::UpVector);
                Normals.Add(FVector::UpVector);
                Normals.Add(FVector::UpVector);

                UV0.Add(FVector2D(0.f, 0.f));
                UV0.Add(FVector2D(1.f, 0.f));
                UV0.Add(FVector2D(1.f, 1.f));
                UV0.Add(FVector2D(0.f, 1.f));

                Tangents.Add(FProcMeshTangent(1, 0, 0));
                Tangents.Add(FProcMeshTangent(1, 0, 0));
                Tangents.Add(FProcMeshTangent(1, 0, 0));
                Tangents.Add(FProcMeshTangent(1, 0, 0));

                VertexColors.Add(FColor::White);
                VertexColors.Add(FColor::White);
                VertexColors.Add(FColor::White);
                VertexColors.Add(FColor::White);
            }
        }
    }

    // Création de la section de maillage procédural
    ProcMeshComp->CreateMeshSection(0, Vertices, Triangles, Normals, UV0, VertexColors, Tangents, true);

    // Optionnel : activer la collision si besoin
    //ProcMeshComp->ContainsPhysicsTriMeshData(true);
}

// -------------------- Cavities Creation --------------------
#pragma region Cavities Creation Methods

void AWorldGeneration::CarveCavities()
{
	for (int32 i = 0; i < NumberOfCavities; ++i)
	{
		int32 CenterX = FMath::RandRange(MapWidth / 4, (MapWidth * 3) / 4);
		int32 CenterY = FMath::RandRange(MapHeight / 4, (MapHeight * 3) / 4);
        
		int32 BaseRadius = FMath::RoundToInt(10 * CavitySizeFactor) + FMath::RandRange(-3, 3);
		float RotationAngle = FMath::RandRange(0.f, 2 * PI);
        
		CarveElongatedCavityAt(CenterX, CenterY, BaseRadius, CavityElongation, RotationAngle);
	}
}

void AWorldGeneration::CarveNaturalCavityAt(float CenterX, float CenterY, float BaseRadius)
{
	float NoiseScale = 5.0f;
	float NoiseAmplitude = 0.3f;
    
	int32 CheckRadius = BaseRadius + 2;
	for (int32 x = CenterX - CheckRadius; x <= CenterX + CheckRadius; ++x)
	{
		for (int32 y = CenterY - CheckRadius; y <= CenterY + CheckRadius; ++y)
		{
			if (x < 0 || y < 0 || x >= MapWidth || y >= MapHeight)
			{
				continue;
			}

			float Angle = FMath::Atan2(y - CenterY, x - CenterX);
			float AngleNorm = (Angle + PI) / (2 * PI);
			
			float NoiseValue = FMath::PerlinNoise1D(AngleNorm * NoiseScale);
			float CurrentRadius = BaseRadius * (1.0f + NoiseAmplitude * NoiseValue);
            
			float Dist = FMath::Sqrt(FMath::Square(x - CenterX) + FMath::Square(y - CenterY));
			if (Dist <= CurrentRadius)
			{
				Grid[x][y] = false;
			}
		}
	}
}

void AWorldGeneration::CarveElongatedCavityAt(float CenterX, float CenterY, float BaseRadius, float ElongationFactor, float RotationAngle)
{
	float a = BaseRadius * ElongationFactor;
	float b = BaseRadius;
	
	int32 CheckRadiusX = FMath::CeilToInt(a) + 2;
	int32 CheckRadiusY = FMath::CeilToInt(b) + 2;
	
	float cosTheta = FMath::Cos(RotationAngle);
	float sinTheta = FMath::Sin(RotationAngle);
	
	for (int32 x = CenterX - CheckRadiusX; x <= CenterX + CheckRadiusX; ++x)
	{
		for (int32 y = CenterY - CheckRadiusY; y <= CenterY + CheckRadiusY; ++y)
		{
			if (x < 0 || y < 0 || x >= MapWidth || y >= MapHeight)
				continue;
			
			float dx = x - CenterX;
			float dy = y - CenterY;

			float xRot = dx * cosTheta + dy * sinTheta;
			float yRot = -dx * sinTheta + dy * cosTheta;
			
			if ((FMath::Square(xRot) / FMath::Square(a)) + (FMath::Square(yRot) / FMath::Square(b)) <= 1.0f)
			{
				Grid[x][y] = false;
			}
		}
	}
}

#pragma endregion


// -------------------- Cavities Creation --------------------
#pragma region Spawn Creation

// Fonction pour générer les spawn points et creuser les cavités associées
void AWorldGeneration::GenerateSpawnPoints()
{
	SpawnPoints.Empty();
    
	int32 TopX    = MapWidth / 2;
	int32 TopY    = MapHeight - 1;
	int32 BottomX = MapWidth / 2;
	int32 BottomY = 0;
	int32 LeftX   = 0;
	int32 LeftY   = MapHeight / 2;
	int32 RightX  = MapWidth - 1;
	int32 RightY  = MapHeight / 2;
    
	SpawnPoints.Add(FVector(TopX * CellSize, TopY * CellSize, 0));
	SpawnPoints.Add(FVector(BottomX * CellSize, BottomY * CellSize, 0));
	SpawnPoints.Add(FVector(LeftX * CellSize, LeftY * CellSize, 0));
	SpawnPoints.Add(FVector(RightX * CellSize, RightY * CellSize, 0));
    
	// Creuser une cavité naturelle à chaque spawn point
	CarveNaturalCavityAt(TopX, TopY, SpawnCavityRadius);
	CarveNaturalCavityAt(BottomX, BottomY, SpawnCavityRadius);
	CarveNaturalCavityAt(LeftX, LeftY, SpawnCavityRadius);
	CarveNaturalCavityAt(RightX, RightY, SpawnCavityRadius);
}

#pragma endregion

