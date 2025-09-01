#include "Widgets/SelectorWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Widgets/PlayerHudWidget.h"
#include "Widgets/SelectorWrapBox.h"


void USelectorWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
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

void USelectorWidget::ShowToolTip(UDataAsset* Data)
{
	if (ParentWidget)
		ParentWidget->ShowToolTip(Data);
}

void USelectorWidget::HideToolTip()
{
	if (ParentWidget)
		ParentWidget->HideToolTip();
}

#pragma endregion
