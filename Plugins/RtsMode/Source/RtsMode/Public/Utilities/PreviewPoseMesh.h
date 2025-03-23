#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PreviewPoseMesh.generated.h"


UCLASS()
class RTSMODE_API APreviewPoseMesh : public AActor
{
	GENERATED_BODY()

public:
	APreviewPoseMesh();
	
	UFUNCTION()
	virtual void ShowPreview(UStaticMesh* NewStaticMesh, FVector NewScale);
	virtual void ShowPreview(USkeletalMesh* NewSkeletalMesh, FVector NewScale);

	UFUNCTION()
	virtual void HidePreview();

	UFUNCTION()
	virtual void CheckIsValidPlacement();

	UFUNCTION()
	virtual bool GetIsValidPlacement();

protected:

	UPROPERTY(EditAnywhere, Category = "Settings|Material")
	UMaterialInstance* HighlightMaterial;

	UPROPERTY()
	UMaterialInstanceDynamic* MaterialInstance;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	USceneComponent* Root;
};
