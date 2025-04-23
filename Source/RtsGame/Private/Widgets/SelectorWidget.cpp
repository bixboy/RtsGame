#include "Widgets/SelectorWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Widgets/SelectorWrapBox.h"
#include "Widgets/ToolTipWidget.h"


void USelectorWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();


	if (UToolTipWidget* ToolTip = CreateWidget<UToolTipWidget>(this, ToolTipClass))
	{
		ToolTip->HideToolTip();
		ToolTip->AddToViewport(9999);
		
		ToolTipInfo = ToolTip;
	}
	
	bIsOpen = true;
	ClearSelectionWidget();
}

void USelectorWidget::ShowUnitEntries(TArray<AActor*> BuildsDataAssets)
{
	if (!WrapBox || BuildsDataAssets.IsEmpty()) return;
	
	WrapBox->ClearWrapper();
	bIsOpen = true;
	
	SelectionBox->SetVisibility(ESlateVisibility::Visible);
	WrapBox->SetupWrapBox(BuildsDataAssets);
	
	StopAllAnimations();
	PlayAnimation(OpenSelection, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f, false);
}

void USelectorWidget::ShowBuildEntries(TArray<AActor*> SelectedBuilds)
{
	if (!WrapBox || SelectedBuilds.IsEmpty()) return;

	WrapBox->ClearWrapper();
	bIsOpen = true;
	
	SelectionBox->SetVisibility(ESlateVisibility::Visible);
	WrapBox->SetupWrapBox(SelectedBuilds);

	StopAllAnimations();
	PlayAnimation(OpenSelection, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f, false);
}



void USelectorWidget::ClearSelectionWidget()
{
	if (!bIsOpen) return;
	
	PlayAnimationForward(CloseSelection);

	WrapBox->ClearWrapper();
	bIsOpen = false;
}


// ---------------------- ToolTip Widgets ----------------------
#pragma region ToolTip Widgets

void USelectorWidget::ShowPendingToolTip()
{
	if (PendingData && ToolTipInfo)
	{
		ToolTipInfo->ShowToolTip(PendingData);
	}
}

void USelectorWidget::ShowToolTip(UDataAsset* Data)
{
	PendingData = Data;
	GetWorld()->GetTimerManager().SetTimer(ToolTipTimerHandle, this, &USelectorWidget::ShowPendingToolTip, 0.7f, false);
}

void USelectorWidget::HideToolTip()
{
	GetWorld()->GetTimerManager().ClearTimer(ToolTipTimerHandle);
	PendingData = nullptr;
	
	if (ToolTipInfo)
	{
		ToolTipInfo->SetVisibility(ESlateVisibility::Collapsed);
	}
}

#pragma endregion
