#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldGeneration.generated.h"


class UProceduralMeshComponent;

UCLASS()
class RTSGAME_API AWorldGeneration : public AActor
{
	GENERATED_BODY()

public:
	AWorldGeneration();

protected:
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	UProceduralMeshComponent* ProcMeshComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings|World Generation")
	int32 MapWidth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings|World Generation")
	int32 MapHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="World Generation")
	int32 MapDepth;

	// Paramètres pour la génération des cavités
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings|World Generation")
	int32 NumberOfCavities;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings|World Generation")
	float CavitySizeFactor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings|World Generation")
	float CavityElongation;

	// Taille d'une cellule pour le mesh procédural
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings|World Generation")
	float CellSize = 100.f;

	// Grille représentant la map (true = roche, false = vide/caverne)
	TArray<TArray<bool>> Grid;

	// Points de départ pour les joueurs (sur chaque bord)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Settings|SpawnPoints")
	TArray<FVector> SpawnPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings|SpawnPoints")
	float SpawnCavityRadius = 5.f;

	UFUNCTION(BlueprintCallable, Category="World Generation")
	void GenerateMap();

	UFUNCTION(BlueprintCallable, Category="World Generation")
	void CarveCavities();

	UFUNCTION(BlueprintCallable, Category="World Generation")
	void CarveNaturalCavityAt(float CenterX, float CenterY, float BaseRadius);

	UFUNCTION(BlueprintCallable, Category="World Generation")
	void CarveElongatedCavityAt(float CenterX, float CenterY, float BaseRadius, float ElongationFactor, float RotationAngle);

	UFUNCTION(BlueprintCallable, Category="World Generation")
	void GenerateMesh();

	UFUNCTION(BlueprintCallable, Category="World Generation")
	void GenerateSpawnPoints();
	
};
