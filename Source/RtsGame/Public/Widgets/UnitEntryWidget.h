#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/DataRts.h"
#include "UnitEntryWidget.generated.h"

class UUnitsProductionDataAsset;
class UCustomButtonWidget;
class ARtsPlayerController;


UCLASS()
class RTSGAME_API UUnitEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void InitEntry(UUnitsProductionDataAsset* DataAsset);
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCustomButtonWidget* BuildButton;

protected:

	UFUNCTION()
	void OnUnitSelected(UCustomButtonWidget* Button, int Index);

	UPROPERTY()
	FUnitsProd UnitData;

	UPROPERTY()
	ARtsPlayerController* PlayerController;
};
