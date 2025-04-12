#include "Widgets/UnitEntryWidget.h"
#include "Components/RtsComponent.h"
#include "Data/UnitsProductionDataAsset.h"
#include "Player/RtsPlayerController.h"
#include "Widget/CustomButtonWidget.h"


void UUnitEntryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	verify((PlayerController = Cast<ARtsPlayerController>(GetWorld()->GetFirstPlayerController())));

	if (UnitButton)
		UnitButton->OnButtonClicked.AddDynamic(this, &UUnitEntryWidget::OnUnitSelected);
}

void UUnitEntryWidget::InitEntry(UUnitsProductionDataAsset* DataAsset)
{
	UnitData = DataAsset;
	UnitButton->SetButtonTexture(UnitData->UnitProduction.UnitImage);
	UnitButton->SetButtonText(FText::FromString(UnitData->UnitProduction.Name));
}

void UUnitEntryWidget::OnUnitSelected(UCustomButtonWidget* Button, int Index)
{
	if (!UnitData->UnitProduction.UnitClass) return;

	PlayerController->RtsComponent->AddUnitToProduction(UnitData);
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, "Unit Selected : " + UnitData->UnitProduction.UnitClass->GetName());
}