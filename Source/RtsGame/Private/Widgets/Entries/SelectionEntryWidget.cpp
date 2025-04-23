#include "Widgets/Entries/SelectionEntryWidget.h"
#include "Data/StructureDataAsset.h"
#include "Data/UnitsProductionDataAsset.h"
#include "Widget/CustomButtonWidget.h"

void USelectionEntryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (CustomButton)
	{
		CustomButton->OnButtonHovered.AddDynamic(this, &USelectionEntryWidget::OnHovered);
	}
}

void USelectionEntryWidget::SetupEntry(int Num)
{
	if (UUnitsProductionDataAsset* UnitData = Cast<UUnitsProductionDataAsset>(DataAsset))
	{
		CustomButton->SetButtonTexture(UnitData->UnitProduction.UnitImage);
		CustomButton->SetButtonText(FText::FromString(FString::FromInt(Num)));
	}

	if (UStructureDataAsset* StructureData = Cast<UStructureDataAsset>(DataAsset))
	{
		CustomButton->SetButtonTexture(StructureData->Structure.Image);
		CustomButton->SetButtonText(FText::FromString(FString::FromInt(Num)));
	}
}

void USelectionEntryWidget::OnHovered(UCustomButtonWidget* Button, int Index)
{
	OnEntryHovered.Broadcast(DataAsset);
}
