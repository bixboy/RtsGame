#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SelectionEntryWidget.generated.h"

class UStructureDataAsset;
class UUnitsProductionDataAsset;
class UCustomButtonWidget;


UCLASS()
class RTSGAME_API USelectionEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void InitEntry(UUnitsProductionDataAsset* DataAsset, int Num);
	void InitEntry(UStructureDataAsset* DataAsset, int Num);
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCustomButtonWidget* Button;
};
