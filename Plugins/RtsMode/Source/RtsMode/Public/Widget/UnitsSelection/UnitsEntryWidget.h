#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UnitsEntryWidget.generated.h"

class UCustomButtonWidget;
class APlayerControllerRts;
class UUnitsSelectionDataAsset;
class ASoldierRts;
class UTextBlock;
class UImage;


UCLASS()
class RTSMODE_API UUnitsEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void InitEntry(UUnitsSelectionDataAsset* DataAsset);
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCustomButtonWidget* UnitButton;

protected:

	UFUNCTION()
	void OnUnitSelected(UCustomButtonWidget* Button, int Index);

	UPROPERTY()
	TSubclassOf<ASoldierRts> UnitClass;

	UPROPERTY()
	APlayerControllerRts* PlayerController;
};
