#include "RtsGame/Public/Widgets/SelectionEntryWidget.h"
#include "Data/StructureDataAsset.h"
#include "Data/UnitsProductionDataAsset.h"
#include "Widget/CustomButtonWidget.h"


void USelectionEntryWidget::InitEntry(UUnitsProductionDataAsset* DataAsset, int Num)
{
	Button->SetButtonTexture(DataAsset->UnitProduction.UnitImage);
	Button->SetButtonText(FText::FromString(FString::FromInt(Num)));
}

void USelectionEntryWidget::InitEntry(UStructureDataAsset* DataAsset, int Num)
{
	Button->SetButtonTexture(DataAsset->Structure.Image);
	Button->SetButtonText(FText::FromString(FString::FromInt(Num)));
}
