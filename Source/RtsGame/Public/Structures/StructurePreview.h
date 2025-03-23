#pragma once
#include "CoreMinimal.h"
#include "StructureBase.h"
#include "Utilities/PreviewPoseMesh.h"
#include "StructurePreview.generated.h"

class AGridManager;

UCLASS()
class RTSGAME_API AStructurePreview : public APreviewPoseMesh
{
	GENERATED_BODY()

public:
	AStructurePreview();

	virtual void CheckIsValidPlacement() override;

	virtual bool GetIsValidPlacement() override;

	UFUNCTION(BlueprintCallable)
	void StartPlacingBuilding(TSubclassOf<AStructureBase> BuildingClass);

	UFUNCTION()
	void EnabledCollision(bool NewEnable);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UGridComponent* GridComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Grid")
	bool bShowGrid = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Grid")
	bool bAreaPlacementValid = false;
	
	UPROPERTY(BlueprintReadOnly)
	int32 BuildingWidth;

	UPROPERTY(BlueprintReadOnly)
	int32 BuildingHeight;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
	UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
	FVector MeshScale = FVector::OneVector;

	UFUNCTION(BlueprintCallable)
	FVector2D ComputeGridSize(float CellSize);
};
