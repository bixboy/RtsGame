#include "Widgets/ToolTipWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/StructureDataAsset.h"
#include "Data/UnitsProductionDataAsset.h"

void UToolTipWidget::ShowToolTip(UDataAsset* DataAsset)
{
	if (!ToolTipImage || !TitleText) return;

	if (UStructureDataAsset* BuildData = Cast<UStructureDataAsset>(DataAsset))
	{
		FStructure Data = BuildData->Structure;
	
		ToolTipImage->SetBrushFromTexture(Data.Image);
		TitleText->SetText(FText::FromString(Data.Name));

		bFollowMouse = true;

		this->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		return;
	}

	if (UUnitsProductionDataAsset* UnitData = Cast<UUnitsProductionDataAsset>(DataAsset))
	{
		FUnitsProd Data = UnitData->UnitProduction;
		
		ToolTipImage->SetBrushFromTexture(Data.UnitImage);
		TitleText->SetText(FText::FromString(Data.Name));

		bFollowMouse = true;

		this->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UToolTipWidget::HideToolTip()
{
	this->SetVisibility(ESlateVisibility::Collapsed);
	bFollowMouse = false;
}

void UToolTipWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bFollowMouse)
	{
		APlayerController* PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
		if (!PC) return;

		float MouseX, MouseY;
		if (PC->GetMousePosition(MouseX, MouseY))
		{
			const FVector2D Offset(10.f, -115.f);
			SetPositionInViewport(FVector2D(MouseX, MouseY) + Offset);
		}
	}
}
