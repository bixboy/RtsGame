#include "Widget/UnitsSelection/UnitsSelectionWidget.h"

#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/ListView.h"
#include "Components/WrapBox.h"
#include "Data/UnitsSelectionDataAsset.h"
#include "Widget/UnitsSelection/UnitsEntryWidget.h"


void UUnitsSelectionWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (Btn_ShowUnitsSelection)
		Btn_ShowUnitsSelection->OnPressed.AddDynamic(this, &UUnitsSelectionWidget::OnShowUnitSelectionPressed);
	
	OnShowUnitSelectionPressed();
	SetupUnitsList();
}

void UUnitsSelectionWidget::SetupUnitsList()
{
	if (!WrapBox) return;

	WrapBox->ClearChildren();

	for (UUnitsSelectionDataAsset* Data : UnitsSelectionDataAssets)
	{
		if (Data)
		{
			UUnitsEntryWidget* UnitWidget = CreateWidget<UUnitsEntryWidget>(GetWorld(), UnitsEntryClass);
			if (UnitWidget)
			{
				UnitWidget->InitEntry(Data);
				WrapBox->AddChild(UnitWidget);
			}
		}
	}
}

void UUnitsSelectionWidget::OnShowUnitSelectionPressed()
{
	if (ListBorder->GetVisibility() == ESlateVisibility::Visible)
	{
		ListBorder->SetVisibility(ESlateVisibility::Collapsed);	
	}
	else
	{
		ListBorder->SetVisibility(ESlateVisibility::Visible);	
	}
}
