#pragma once
#include "CoreMinimal.h"
#include "Data/DataRts.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Selectable.h"
#include "Manager/GridManager.h"
#include "StructureBase.generated.h"

class UHealthComponent;
class UStructureDataAsset;
class AGridManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStructureSelectedDelegate, bool, bIsSelected);


UCLASS()
class RTSGAME_API AStructureBase : public AActor, public ISelectable
{
	GENERATED_BODY()

public:
	AStructureBase();

	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION(BlueprintCallable)
	FStructure GetStructureInfo() const;

protected:
	virtual void BeginPlay() override;
	
// ----------------------- Setup -----------------------
#pragma region Components
	
protected:	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Base")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Base")
	UHealthComponent* HealthComp;

public:
	UStaticMesh* GetStaticMesh();

#pragma endregion

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Base")
	UStructureDataAsset* StructureData;

#pragma region Interfaces
	
public:
	/*- Function -*/
	virtual void Select() override;
	virtual void Deselect() override;
	virtual void Highlight(const bool Highlight) override;

	virtual ESelectionType GetSelectionType_Implementation() override;

	UFUNCTION()
	bool GetIsSelected() const;

	UPROPERTY()
	FStructureSelectedDelegate OnSelected;

protected:
	/*- Variables -*/
	UPROPERTY()
	bool Selected;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Base")
	ESelectionType Type = ESelectionType::Structure;
	

#pragma endregion	
	
};
