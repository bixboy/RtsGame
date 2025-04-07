#include "Widgets/UnitEntryWidget.h"
#include "Components/RtsComponent.h"
#include "Data/UnitsProductionDataAsset.h"
#include "Player/RtsPlayerController.h"
#include "Widget/CustomButtonWidget.h"


void UUnitEntryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	verify((PlayerController = Cast<ARtsPlayerController>(GetWorld()->GetFirstPlayerController())));

	if (BuildButton)
		BuildButton->OnButtonClicked.AddDynamic(this, &UUnitEntryWidget::OnUnitSelected);
}

void UUnitEntryWidget::InitEntry(UUnitsProductionDataAsset* DataAsset)
{
	UnitData = DataAsset->UnitProduction;
	BuildButton->SetButtonTexture(UnitData.UnitImage);
	BuildButton->SetButtonText(FText::FromString(UnitData.Name));
}

void UUnitEntryWidget::OnUnitSelected(UCustomButtonWidget* Button, int Index)
{
	if (!UnitData.UnitClass) return;

	//PlayerController->RtsComponent->ChangeUnitClass(UnitData);
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, "Unit Selected : " + UnitData.UnitClass->GetName());
}