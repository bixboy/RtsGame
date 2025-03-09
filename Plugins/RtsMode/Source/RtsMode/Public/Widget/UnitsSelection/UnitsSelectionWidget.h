#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UnitsSelectionWidget.generated.h"

class UUnitsEntryWidget;
class UWrapBox;
class UBorder;
class UButton;
class UUnitsSelectionDataAsset;


UCLASS()
class RTSMODE_API UUnitsSelectionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	
	UFUNCTION()
	void SetupUnitsList();
	
protected:

	UFUNCTION()
	void OnShowUnitSelectionPressed();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	TSubclassOf<UUnitsEntryWidget> UnitsEntryClass;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Btn_ShowUnitsSelection;
	
	UPROPERTY(EditAnywhere, Category = "Settings")
	TArray<UUnitsSelectionDataAsset*> UnitsSelectionDataAssets;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWrapBox* WrapBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBorder* ListBorder;
	
};
