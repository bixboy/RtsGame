#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UnitsEntryWidget.generated.h"

class APlayerControllerRts;
class UUnitsSelectionDataAsset;
class ASoldierRts;
class UButton;
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

protected:

	UFUNCTION()
	void OnUnitSelected();
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* UnitImage;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* UnitName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* UnitButton;

	UPROPERTY()
	TSubclassOf<ASoldierRts> UnitClass;

	UPROPERTY()
	APlayerControllerRts* PlayerController;
};
