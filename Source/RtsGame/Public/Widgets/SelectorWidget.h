#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget/CustomButtonWidget.h"
#include "SelectorWidget.generated.h"


class UUnitEntryWidget;
class UUnitsProductionDataAsset;
class UStructureDataAsset;
class UBuildsEntry;
class UBorder;
class UWrapBox;

UCLASS()
class RTSGAME_API USelectorWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void NativeOnInitialized();

protected:
	
	UFUNCTION()
	void SwitchToBuild(TArray<UStructureDataAsset*> BuildsDataAssets);

	UFUNCTION()
	void SwitchToUnit(TArray<UUnitsProductionDataAsset*> UnitsDataAssets);

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWrapBox* WrapBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBorder* ListBorder;

	UPROPERTY()
	TArray<UBuildsEntry*> BuildEntryList;

	UPROPERTY()
	TArray<UUnitEntryWidget*> UnitEntryList;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	TSubclassOf<UBuildsEntry> BuildsEntryClass;
};
