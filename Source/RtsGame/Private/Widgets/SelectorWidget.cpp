#include "Widgets/SelectorWidget.h"

#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/WrapBox.h"
#include "Data/StructureDataAsset.h"
#include "Data/UnitsProductionDataAsset.h"
#include "Widgets/BuildsEntry.h"
#include "Widgets/SelectionEntryWidget.h"
#include "Widgets/UnitEntryWidget.h"


void USelectorWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void USelectorWidget::SwitchToBuild(TArray<UStructureDataAsset*> BuildsDataAssets)
{
	if (!WrapBox || BuildsDataAssets.IsEmpty()) return;

	ListBorder->SetVisibility(ESlateVisibility::Visible);
	SelectionBorder->SetVisibility(ESlateVisibility::Visible);

	WrapBox->ClearChildren();
	UnitEntryList.Empty();
	
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
	if (!WrapBox || UnitsDataAssets.IsEmpty()) return;
	
	ListBorder->SetVisibility(ESlateVisibility::Visible);
	SelectionBorder->SetVisibility(ESlateVisibility::Collapsed);

	WrapBox->ClearChildren();
	BuildEntryList.Empty();
	
	for (UUnitsProductionDataAsset* Data : UnitsDataAssets)
	{
		if (Data)
		{
			if (UUnitEntryWidget* UnitWidget = CreateWidget<UUnitEntryWidget>(GetWorld(), UnitEntryClass))
			{
				UnitWidget->InitEntry(Data);
				
				WrapBox->AddChild(UnitWidget);
				UnitEntryList.Add(UnitWidget);
			}
		}
	}
}

void USelectorWidget::ClearSelectionWidget()
{
	WrapBox->ClearChildren();
	
	BuildEntryList.Empty();
	UnitEntryList.Empty();

	ListBorder->SetVisibility(ESlateVisibility::Collapsed);
	SelectionBorder->SetVisibility(ESlateVisibility::Collapsed);
}

void USelectorWidget::UpdateSelection(const TMap<UUnitsProductionDataAsset*, FGroupedActors>& GroupedSelection)
{
	if (GroupedSelection.IsEmpty()) return;
    
	SelectionWrapBox->ClearChildren();

	for (const TPair<UUnitsProductionDataAsset*, FGroupedActors>& Pair : GroupedSelection)
	{
		UUnitsProductionDataAsset* DataAsset = Pair.Key;
		const FGroupedActors& Group = Pair.Value;

		if (DataAsset)
		{
			if (USelectionEntryWidget* SelectionWidget = CreateWidget<USelectionEntryWidget>(GetWorld(), SelectionEntryClass))
			{
				SelectionWidget->InitEntry(DataAsset, Group.Actors.Num());
				SelectionWrapBox->AddChild(SelectionWidget);
			}
		}
	}
}
