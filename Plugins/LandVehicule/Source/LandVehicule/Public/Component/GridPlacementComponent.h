#pragma once
#include "CoreMinimal.h"
#include "GridPlacementComponent.generated.h"

class UProceduralMeshComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LANDVEHICULE_API UGridPlacementComponent : public USceneComponent
{
	GENERATED_BODY()
	
public:
	UGridPlacementComponent();

protected:

#if WITH_EDITOR
	
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
	virtual void OnRegister() override;

	void BuildGridMesh();
#endif

private:
	
	UPROPERTY(EditAnywhere, Category="Grid")
	float CellSize = 100.f;

	UPROPERTY(EditAnywhere, Category="Grid")
	int32 GridCountX = 10;

	UPROPERTY(EditAnywhere, Category="Grid")
	int32 GridCountY = 10;

	UPROPERTY(EditAnywhere, Category="Grid")
	float GridHeight = 0.f;

	UPROPERTY(EditAnywhere, Category="Grid")
	float LineThickness = 5.f;

	UPROPERTY(EditAnywhere, Category="Grid")
	UMaterialInterface* LineMaterial = nullptr;

	UPROPERTY(EditAnywhere, Category="Grid")
	bool bShowGrid = true;

	UPROPERTY()
	UProceduralMeshComponent* ProcMesh;

};
