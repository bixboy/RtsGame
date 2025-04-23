#include "Widgets/UnitsInfo/UnitsInfoBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "Data/UnitsProductionDataAsset.h"
#include "Widgets/Entries/BuildsEntry.h"
#include "Data/StructureDataAsset.h"
#include "Widgets/SelectorWrapBox.h"


void UUnitsInfoBox::SetupUnitInfos(UUnitsProductionDataAsset* UnitData)
{
	if (UnitData)
	{
		UnitImage->SetBrushFromTexture(UnitData->UnitProduction.UnitImage);

		UnitName->SetText(FText::FromString(UnitData->UnitProduction.Name));
		UnitDesc->SetText(FText::FromString(UnitData->UnitProduction.Description));

		WrapBox->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UUnitsInfoBox::ShowBuildList(TArray<UStructureDataAsset*> BuildList, USelectorWrapBox* Owner)
{
	WrapBox->ClearChildren();
	WrapBox->SetVisibility(ESlateVisibility::Visible);
	
	for (UStructureDataAsset* Data : BuildList)
	{
		if (Data)
		{
			if (UBuildsEntry* BuildEntry = CreateWidget<UBuildsEntry>(GetWorld(), BuildsEntryClass))
			{
				BuildEntry->InitializeEntry(Data);
				
				Owner->AddDelegateToolTip(BuildEntry);
				
				WrapBox->AddChild(BuildEntry);
			}
		}
	}
}
