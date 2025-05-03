#include "Widgets/BuildSelector.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/WrapBox.h"
#include "Data/StructureDataAsset.h"
#include "Widget/CustomButtonWidget.h"
#include "Widgets/Entries/BuildsEntry.h"


void UBuildSelector::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (Btn_ShowBuildSelector)
		Btn_ShowBuildSelector->OnPressed.AddDynamic(this, &UBuildSelector::OnShowButtonPressed);
	
	OnShowButtonPressed();
	SetupBuildsList();
}

void UBuildSelector::SetupBuildsList()
{
	if (!WrapBox || BuildsDataAssets.IsEmpty()) return;

	WrapBox->ClearChildren();

	for (UStructureDataAsset* Data : BuildsDataAssets)
	{
		if (Data)
		{
			UBuildsEntry* UnitWidget = CreateWidget<UBuildsEntry>(GetWorld(), BuildsEntryClass);
			if (UnitWidget)
			{
				UnitWidget->InitializeEntry(Data);
				
				WrapBox->AddChild(UnitWidget);
				EntryList.Add(UnitWidget);
				
				UnitWidget->CustomButton->OnButtonClicked.AddDynamic(this, &UBuildSelector::OnBuildSelected);
			}
		}
	}
}

void UBuildSelector::OnShowButtonPressed()
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

void UBuildSelector::OnBuildSelected(UCustomButtonWidget* Button, int Index)
{
	for (UBuildsEntry* EntryWidget : EntryList)
	{
		if (EntryWidget)
			EntryWidget->CustomButton->ToggleButtonIsSelected(false);
	}

	if (Button)
		Button->ToggleButtonIsSelected(true);
}
