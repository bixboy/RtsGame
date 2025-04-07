#include "Widgets/SelectorWidget.h"
#include "Components/WrapBox.h"
#include "Data/StructureDataAsset.h"
#include "Widgets/BuildsEntry.h"
#include "Widgets/UnitEntryWidget.h"


void USelectorWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void USelectorWidget::SwitchToBuild(TArray<UStructureDataAsset*> BuildsDataAssets)
{
	if (!WrapBox) return;

	WrapBox->ClearChildren();

	for (UStructureDataAsset* Data : BuildsDataAssets)
	{
		if (Data)
		{
			if (UBuildsEntry* UnitWidget = CreateWidget<UBuildsEntry>(GetWorld(), BuildsEntryClass))
			{
				UnitWidget->InitEntry(Data);
				
				WrapBox->AddChild(UnitWidget);
				BuildEntryList.Add(UnitWidget);
				
			}
		}
	}
}


void USelectorWidget::SwitchToUnit(TArray<UUnitsProductionDataAsset*> UnitsDataAssets)
{
	if (!WrapBox) return;

	WrapBox->ClearChildren();

	for (UUnitsProductionDataAsset* Data : UnitsDataAssets)
	{
		if (Data)
		{
			if (UUnitEntryWidget* UnitWidget = CreateWidget<UUnitEntryWidget>(GetWorld(), BuildsEntryClass))
			{
				UnitWidget->InitEntry(Data);
				
				WrapBox->AddChild(UnitWidget);
				UnitEntryList.Add(UnitWidget);
			}
		}
	}
}
