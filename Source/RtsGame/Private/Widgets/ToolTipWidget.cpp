#include "Widgets/ToolTipWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/StructureDataAsset.h"
#include "Data/UnitsProductionDataAsset.h"

void UToolTipWidget::ShowToolTip(UDataAsset* DataAsset)
{
	if (!ToolTipImage || !TitleText) return;

	if (!PC)
	{
		PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
	}

	if (PC)
	{
		float MouseX, MouseY;
		if (PC->GetMousePosition(MouseX, MouseY))
		{
			const FVector2D Offset(10.f, -100.f);
			SetPositionInViewport(FVector2D(MouseX, MouseY) + Offset);
		}

		if (UStructureDataAsset* BuildData = Cast<UStructureDataAsset>(DataAsset))
		{
			bFollowMouse = true;
		
			FStructure Data = BuildData->Structure;
	
			ToolTipImage->SetBrushFromTexture(Data.Image);
			TitleText->SetText(FText::FromString(Data.Name));

			this->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			return;
		}

		if (UUnitsProductionDataAsset* UnitData = Cast<UUnitsProductionDataAsset>(DataAsset))
		{
			bFollowMouse = true;
		
			FUnitsProd Data = UnitData->UnitProduction;
		
			ToolTipImage->SetBrushFromTexture(Data.UnitImage);
			TitleText->SetText(FText::FromString(Data.Name));

			this->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}	
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

	if (bFollowMouse && PC)
	{
		float MouseX, MouseY;
		if (PC->GetMousePosition(MouseX, MouseY))
		{
			const FVector2D Offset(10.f, -100.f);
			SetPositionInViewport(FVector2D(MouseX, MouseY) + Offset);
		}
	}
}
