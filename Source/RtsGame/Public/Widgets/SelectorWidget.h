#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget/CustomButtonWidget.h"
#include "SelectorWidget.generated.h"

class USelectionEntryWidget;
class UUnitEntryWidget;
class UUnitsProductionDataAsset;
class UStructureDataAsset;
class UBuildsEntry;
class UBorder;
class UWrapBox;

USTRUCT(BlueprintType)
struct FGroupedActors
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> Actors;
};


UCLASS()
class RTSGAME_API USelectorWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void NativeOnInitialized();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	TArray<UStructureDataAsset*> Builds;

	UFUNCTION()
	void SwitchToBuild(TArray<UStructureDataAsset*> BuildsDataAssets);

	UFUNCTION()
	void SwitchToUnit(TArray<UUnitsProductionDataAsset*> UnitsDataAssets);

	UFUNCTION()
	void ClearSelectionWidget();

	UFUNCTION()
	void UpdateSelection(const TMap<UUnitsProductionDataAsset*, FGroupedActors>& GroupedSelection);

protected:
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWrapBox* WrapBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBorder* ListBorder;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWrapBox* SelectionWrapBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBorder* SelectionBorder;

	UPROPERTY()
	TArray<UBuildsEntry*> BuildEntryList;

	UPROPERTY()
	TArray<UUnitEntryWidget*> UnitEntryList;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	TSubclassOf<UBuildsEntry> BuildsEntryClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	TSubclassOf<UUnitEntryWidget> UnitEntryClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	TSubclassOf<USelectionEntryWidget> SelectionEntryClass;
};
