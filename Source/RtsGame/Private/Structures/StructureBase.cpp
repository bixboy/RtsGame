#include "Structures/StructureBase.h"
#include "Components/HealthComponent.h"
#include "Data/StructureDataAsset.h"


// ----------------------- Setup -----------------------
#pragma region Setup

AStructureBase::AStructureBase()
{
	PrimaryActorTick.bCanEverTick = true;

	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComp"));

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
}

void AStructureBase::OnConstruction(const FTransform& Transform)
{
	if (MeshComp && StructureData && StructureData->Structure.StructureMesh)
	{
		MeshComp->SetStaticMesh(StructureData->Structure.StructureMesh);
	}
	else if (MeshComp)
	{
		MeshComp->SetStaticMesh(nullptr);
	}
	
	Super::OnConstruction(Transform);
}

void AStructureBase::BeginPlay()
{
	Super::BeginPlay();
}

#pragma endregion


FStructure AStructureBase::GetStructureInfo() const
{
	if (StructureData)
		return StructureData->Structure;

	return FStructure();
}

UStaticMesh* AStructureBase::GetStaticMesh()
{
	return MeshComp->GetStaticMesh();
}


// ----------------------- Selection -----------------------
#pragma region Selection

void AStructureBase::Select()
{
	Selected = true;
	Highlight(Selected);

	OnSelected.Broadcast(Selected);
}

void AStructureBase::Deselect()
{
	Selected = false;
	Highlight(Selected);

	OnSelected.Broadcast(Selected);
}

void AStructureBase::Highlight(const bool Highlight)
{
	TArray<UPrimitiveComponent*> Components;
	GetComponents<UPrimitiveComponent>(Components);
	for (UPrimitiveComponent* VisualComp : Components)
	{
		if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(VisualComp))
		{
			Prim->SetRenderCustomDepth(Highlight);
		}
	}
}

ESelectionType AStructureBase::GetSelectionType_Implementation()
{
	return Type;
}

bool AStructureBase::GetIsSelected() const
{
	return Selected;
}

#pragma endregion

