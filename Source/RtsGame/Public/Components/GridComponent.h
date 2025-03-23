#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GridComponent.generated.h"

class UBoxComponent;
class UProceduralMeshComponent;

USTRUCT(BlueprintType)
struct FGridCell
{
	GENERATED_BODY()
	
	UPROPERTY()
	FVector2D Position;

	UPROPERTY()
	UMaterialInstanceDynamic* MaterialInstance;

	UPROPERTY()
	int Index;

	UPROPERTY()
	bool bOccupied = false;
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RTSGAME_API UGridComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGridComponent();

	virtual void OnRegister() override;

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ---------- Settings ----------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
	bool bCollisionEnabled = false;
	
	// ---------- Size ----------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings|Size")
	int32 GridWidth = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings|Size")
	int32 GridHeight = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings|Size")
	float CellSize = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings|Size")
	float LineThickness = 10.f;

	// ---------- Color ----------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings|Color")
	UMaterial* Material;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings|Color")
	FLinearColor LineColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings|Color")
	FLinearColor SelectionColor;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings|Color")
	float LineOpacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings|Color")
	float SelectionOpacity;
		
	/*--------- Function ---------*/
public:
	UFUNCTION()
	void SetupLines(float Width = 0.f, float Height = 0.f);

	UFUNCTION()
	float GetCellSize();
	
protected:
	UFUNCTION()
	void CreateLine(FVector Start, FVector End, float Thickness, TArray<FVector>& Vertices, TArray<int>& Triangles);

	UFUNCTION()
	float GetGridWidth();

	UFUNCTION()
	float GetGridHeight();

	UFUNCTION()
	UMaterialInstanceDynamic* CreateMaterial(FLinearColor Color, float Opacity);
	

	UFUNCTION()
	bool GetTileToGrid(int Row, int Column, bool bCenter, FVector2D& GridLocation);

	UFUNCTION()
	TArray<FVector2D> GetTilesForActor(AActor* Actor);

public:

	UFUNCTION()
	void CreateSelectionCells(TArray<FVector2D> CellsToDisplay);

	UFUNCTION()
	bool GetCellsIsOccupied(bool bAllCells = false);

	UFUNCTION()
	bool GetLocationToTile(FVector Location, int& Row, int& Column);

	UFUNCTION()
	void InvertColorCells(bool bReverseIfAlreadyReversed = false);

	UFUNCTION()
	bool GetCollisionIsEnabled();

	UFUNCTION()
	void EnabledCollision(bool NewEnable);

	UFUNCTION()
	void SetActiveCollision(bool bNewActive);
	
protected:

	UFUNCTION()
	void CheckOverlappingCells();
	
	UFUNCTION()
	bool GetTileIsValid(int Row, int Column);

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
					int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	/*-------- Variables --------*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UProceduralMeshComponent* LineProceduralMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UProceduralMeshComponent* SelectionProceduralMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* GridCollisionBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FGridCell> GridCells;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FVector2D> GridOccupancy;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TArray<AActor*> OverlappingActors;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bActorInGrid = false;

	UPROPERTY()
	bool bColorInverted = false;
};
