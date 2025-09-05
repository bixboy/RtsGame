#include "Component/GridPlacementComponent.h"
#include "ProceduralMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

UGridPlacementComponent::UGridPlacementComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	ProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GridProcMesh"));
	ProcMesh->SetupAttachment(this);
	
	ProcMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ProcMesh->bUseAsyncCooking = true;
}

#if WITH_EDITOR
void UGridPlacementComponent::OnRegister()
{
	Super::OnRegister();
	BuildGridMesh();
}

void UGridPlacementComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	BuildGridMesh();
}
#endif

void UGridPlacementComponent::BuildGridMesh()
{
	if (!bShowGrid || !ProcMesh)
	{
		ProcMesh->ClearAllMeshSections();
		return;
	}

	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UV0;
	TArray<FColor> VertexColors;
	TArray<FProcMeshTangent> Tangents;

	auto CreateLine = [&](const FVector& Start, const FVector& End)
	{
		FVector Dir = (End - Start).GetSafeNormal();
		FVector Right = FVector::CrossProduct(Dir, FVector::UpVector);

		FVector P0 = Start + Right * (LineThickness * 0.5f);
		FVector P1 = End   + Right * (LineThickness * 0.5f);
		FVector P2 = Start - Right * (LineThickness * 0.5f);
		FVector P3 = End   - Right * (LineThickness * 0.5f);

		int32 BaseIndex = Vertices.Num();
		Vertices.Append({P0, P1, P2, P3});
		Triangles.Append({
			BaseIndex + 0, BaseIndex + 1, BaseIndex + 2,
			BaseIndex + 2, BaseIndex + 1, BaseIndex + 3
		});
		
		for (int i=0; i<4; i++)
			Normals.Add(FVector::UpVector);
		
		for (int i=0; i<4; i++)
		{
			UV0.Add(FVector2D::ZeroVector);
			VertexColors.Add(FColor::White); Tangents.Add({Dir, false});
		}
	};

	const float TotalX = GridCountX * CellSize;
	const float TotalY = GridCountY * CellSize;
	
	const FTransform& T = GetComponentTransform();
	const float HalfX = TotalX * 0.5f;
	const float HalfY = TotalY * 0.5f;

	for (int32 ix = 0; ix <= GridCountX; ++ix)
	{
		float X = ix * CellSize - HalfX;
		FVector Start = FVector(X, -HalfY, GridHeight);
		FVector End   = FVector(X,  HalfY, GridHeight);

		Start = T.TransformPosition(Start);
		End   = T.TransformPosition(End);
		
		CreateLine(Start, End);
	}
	for (int32 iy = 0; iy <= GridCountY; ++iy)
	{
		float Y = iy * CellSize - HalfY;
		FVector Start = FVector(-HalfX, Y, GridHeight);
		FVector End   = FVector( HalfX, Y, GridHeight);
		
		Start = T.TransformPosition(Start);
		End   = T.TransformPosition(End);
		CreateLine(Start, End);
	}

	ProcMesh->ClearAllMeshSections();
	ProcMesh->CreateMeshSection(0, Vertices, Triangles, Normals, UV0, VertexColors, Tangents, false);
	
	if (LineMaterial) ProcMesh->SetMaterial(0, LineMaterial);
}