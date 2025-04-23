#pragma once
#include "CoreMinimal.h"
#include "Widgets/Entries/EntryWidget.h"
#include "SelectionEntryWidget.generated.h"

class UStructureDataAsset;
class UUnitsProductionDataAsset;
class UCustomButtonWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSelectionEntryHovered, UUnitsProductionDataAsset*, Data);


UCLASS()
class RTSGAME_API USelectionEntryWidget : public UEntryWidget
{
	GENERATED_BODY()

public:
	
	UFUNCTION()
	void SetupEntry(int Num);


protected:
	
	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void OnHovered(UCustomButtonWidget* Button, int Index);
	
};
