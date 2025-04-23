#pragma once
#include "CoreMinimal.h"
#include "Widgets/Entries/EntryWidget.h"
#include "UnitEntryWidget.generated.h"

class UProgressBar;
class AUnitProduction;
class UTextBlock;
class UUnitsProductionDataAsset;


UCLASS()
class RTSGAME_API UUnitEntryWidget : public UEntryWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	
	UFUNCTION()
	void SetupEntry(AActor* Build, int UnitNumProd, bool ShowCount = true);

	UFUNCTION()
	void UpdateEntry();

protected:
	
	UFUNCTION()
	void OnUnitSelected(UCustomButtonWidget* Button, int Index);
	
	UFUNCTION()
	void UpdateProdProgress(float NewProgress, UUnitsProductionDataAsset* UnitInProduct);

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* UnitProdNum;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UProgressBar* ProgressBar;

	UPROPERTY()
	UUnitsProductionDataAsset* UnitData;

	UPROPERTY()
	AUnitProduction* BuildProduction;

	UPROPERTY()
	int UnitNumberProd;

	UPROPERTY()
	bool bShowCount = true;
};
