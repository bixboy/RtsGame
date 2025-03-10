#include "Utilities/PreviewPoseMesh.h"


APreviewPoseMesh::APreviewPoseMesh()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(Root);
	
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(Root);
	
	bReplicates = false;
}

void APreviewPoseMesh::ShowPreview(UStaticMesh* NewStaticMesh, FVector NewScale)
{
	SetActorHiddenInGame(false);
	
	if (SkeletalMesh->GetSkeletalMeshAsset())
		SkeletalMesh->SetSkeletalMesh(nullptr);

	StaticMesh->SetStaticMesh(NewStaticMesh);
	StaticMesh->SetRelativeScale3D(NewScale);
	
	if (HighlightMaterial)
	{
		MaterialInstance = UMaterialInstanceDynamic::Create(HighlightMaterial, this);
		StaticMesh->SetOverlayMaterial(MaterialInstance);
	}
}

void APreviewPoseMesh::ShowPreview(USkeletalMesh* NewSkeletalMesh, FVector NewScale)
{
	SetActorHiddenInGame(false);
	
	if (StaticMesh->GetStaticMesh())
		StaticMesh->SetStaticMesh(nullptr);

	SkeletalMesh->SetSkeletalMesh(NewSkeletalMesh);
	SkeletalMesh->SetRelativeScale3D(NewScale);

	if (HighlightMaterial)
	{
		MaterialInstance = UMaterialInstanceDynamic::Create(HighlightMaterial, this);
		SkeletalMesh->SetOverlayMaterial(MaterialInstance);
	}
}

void APreviewPoseMesh::HidePreview()
{
	if (StaticMesh->GetStaticMesh())
		StaticMesh->SetStaticMesh(nullptr);

	if (SkeletalMesh->GetSkeletalMeshAsset())
		SkeletalMesh->SetSkeletalMesh(nullptr);

	SetActorHiddenInGame(true);
}

