#include "Widgets/PlayerHudWidget.h"

#include "Widgets/SelectorWidget.h"
#include "Widgets/ToolTipWidget.h"


void UPlayerHudWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (UToolTipWidget* ToolTip = CreateWidget<UToolTipWidget>(this, ToolTipClass))
	{
		ToolTip->HideToolTip();
		ToolTip->AddToViewport(9999);
		
		ToolTipInfo = ToolTip;
	}

	if (SelectorWidget)
		SelectorWidget->SetParentWidget(this);
}

void UPlayerHudWidget::ShowPendingToolTip()
{
	if (PendingData && ToolTipInfo)
	{
		ToolTipInfo->ShowToolTip(PendingData);
	}
}

void UPlayerHudWidget::ShowToolTip(UDataAsset* Data)
{
	PendingData = Data;
	GetWorld()->GetTimerManager().SetTimer(ToolTipTimerHandle, this, &UPlayerHudWidget::ShowPendingToolTip, 0.7f, false);
}

void UPlayerHudWidget::HideToolTip()
{
	GetWorld()->GetTimerManager().ClearTimer(ToolTipTimerHandle);
	PendingData = nullptr;
	
	if (ToolTipInfo)
	{
		ToolTipInfo->SetVisibility(ESlateVisibility::Collapsed);
	}
}
