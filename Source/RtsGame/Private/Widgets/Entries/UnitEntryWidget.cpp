#include "Widgets/Entries/UnitEntryWidget.h"
#include "Components/ProgressBar.h"
#include "Components/RtsComponent.h"
#include "Components/TextBlock.h"
#include "Data/UnitsProductionDataAsset.h"
#include "Player/RtsPlayerController.h"
#include "Widget/CustomButtonWidget.h"


void UUnitEntryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (CustomButton)
	{
		CustomButton->OnButtonClicked.AddDynamic(this, &UUnitEntryWidget::OnUnitSelected);
	}
}

void UUnitEntryWidget::SetupEntry(AActor* Build, int UnitNumProd, bool ShowCount)
{
	UnitData = Cast<UUnitsProductionDataAsset>(DataAsset);
	
	CustomButton->SetButtonTexture(UnitData->UnitProduction.UnitImage);
	CustomButton->SetButtonText(FText::FromString(UnitData->UnitProduction.Name));
	
	BuildProduction = Cast<AUnitProduction>(Build);
	bShowCount = ShowCount;
	
	if (bShowCount && BuildProduction)
	{
		UnitNumberProd = UnitNumProd;
		if (UnitNumberProd > 0)
		{
			UnitProdNum->SetVisibility(ESlateVisibility::Visible);
			UnitProdNum->SetText(FText::FromString(FString::FromInt(UnitNumberProd)));

			ProgressBar->SetVisibility(ESlateVisibility::Visible);
			ProgressBar->SetPercent(IUnitProductionInterface::Execute_GetProductionProgress(BuildProduction));
		}
	}

	if (BuildProduction)
	{
		BuildProduction->OnProductionProgress.RemoveDynamic(this, &UUnitEntryWidget::UpdateProdProgress);
		BuildProduction->OnProductionProgress.AddDynamic(this, &UUnitEntryWidget::UpdateProdProgress);
	}
}

void UUnitEntryWidget::UpdateEntry()
{
	if (BuildProduction && UnitData->UnitProduction.UnitClass)
	{
		int ProductionNum = IUnitProductionInterface::Execute_GetUnitsInQueueByClass(BuildProduction, UnitData->UnitProduction.UnitClass).Num();
		
		UnitNumberProd = ProductionNum;
		UnitProdNum->SetText(FText::FromString(FString::FromInt(UnitNumberProd)));

		if (UnitNumberProd == 0)
		{
			UnitProdNum->SetVisibility(ESlateVisibility::Collapsed);
			ProgressBar->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UUnitEntryWidget::UpdateProdProgress(float NewProgress, UUnitsProductionDataAsset* UnitInProduct)
{
	if (UnitInProduct == UnitData)
	{
		ProgressBar->SetVisibility(ESlateVisibility::Visible);
		ProgressBar->SetPercent(NewProgress);	
	}
}

void UUnitEntryWidget::OnUnitSelected(UCustomButtonWidget* Button, int Index)
{
	if (!UnitData->UnitProduction.UnitClass) return;
	
	PlayerController->RtsComponent->AddUnitToProduction(UnitData);

	if (bShowCount)
	{
		UnitNumberProd ++;
		FString UnitNumber = FString::FromInt(UnitNumberProd);
		
		UnitProdNum->SetVisibility(ESlateVisibility::Visible);
		UnitProdNum->SetText(FText::FromString(UnitNumber));

		if (BuildProduction->GetSelectedUnit() == UnitData)
		{
			ProgressBar->SetVisibility(ESlateVisibility::Visible);	
		}
	}
}
