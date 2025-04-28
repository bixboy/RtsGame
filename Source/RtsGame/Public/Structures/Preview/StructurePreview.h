#pragma once
#include "CoreMinimal.h"
#include "Data/DataRts.h"
#include "Utilities/PreviewPoseMesh.h"
#include "StructurePreview.generated.h"

class ARtsPlayer;
class UGridComponent;
class AGridManager;

UCLASS()
class RTSGAME_API AStructurePreview : public APreviewPoseMesh
{
	GENERATED_BODY()

public:
	AStructurePreview();

	virtual void CheckIsValidPlacement() override;

	virtual bool GetIsValidPlacement() override;

	UFUNCTION()
	void StartPlacingBuilding(const FStructure BuildData);

	UFUNCTION()
	void EnabledCollision(bool NewEnable);

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float Delta) override;

	UPROPERTY()
	ARtsPlayer* Player;


// ========= Preview =========
	
	UPROPERTY(BlueprintReadOnly)
	int32 BuildingWidth;

	UPROPERTY(BlueprintReadOnly)
	int32 BuildingHeight;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings|Grid")
	UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings|Grid")
	FVector MeshScale = FVector::OneVector;

	
// ========= Grid =========

	UFUNCTION(BlueprintCallable)
	FVector2D ComputeGridSize(float CellSize);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UGridComponent* GridComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Grid")
	bool bShowGrid = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Grid")
	bool bAreaPlacementValid = false;

	
// ========= Wall Preview =========
public:
	
	UFUNCTION(BlueprintCallable, Category="Wall")
	void StartWallPreview(const FStructure BuildData);

	UFUNCTION(BlueprintCallable, Category="Wall")
	void ShowWallPreview(FVector MouseLocation);

	UFUNCTION(BlueprintCallable, Category="Wall")
	void StopWallPreview();

	UFUNCTION(BlueprintCallable, Category="Wall")
	TArray<FTransform> ConfirmWallPreview();

	
protected:
	
	UPROPERTY(EditAnywhere, Category = "Settings|Wall")
	TSubclassOf<AStructurePreview> WallPreviewClass;

	UPROPERTY(VisibleAnywhere)
	UInstancedStaticMeshComponent* PreviewValidInstance;

	UPROPERTY(VisibleAnywhere)
	UInstancedStaticMeshComponent* PreviewInvalidInstance;


	UPROPERTY()
	bool bIsPreviewingWall = false;
	
	UPROPERTY()
	FVector StartTransform;

	UPROPERTY()
	float CachedSegmentLength = 0.f;

	UPROPERTY()
	TArray<AActor*> WallsPreview;
	
	FVector CachedMeshScale = FVector(1.f);
};
