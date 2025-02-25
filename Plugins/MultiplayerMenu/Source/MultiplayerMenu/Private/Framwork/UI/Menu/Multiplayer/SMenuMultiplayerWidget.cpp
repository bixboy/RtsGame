#include "Framwork/UI/Menu/Multiplayer/SMenuMultiplayerWidget.h"
#include "PrimaryGameLayout.h"
#include "Components/Overlay.h"
#include "Framwork/UI/Menu/SButtonBaseWidget.h"
#include "Framwork/UI/Menu/Multiplayer/SMenuhostSessionWidget.h"
#include "Framwork/UI/Menu/Multiplayer/SMenuJoinSessionWidget.h"
#include "Kismet/KismetSystemLibrary.h"

void USMenuMultiplayerWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (HostButton && HostWidget)
	{
		HostButton->OnClicked().AddUObject(this, &USMenuMultiplayerWidget::OnHostButtonClicked);
		HostWidget->BackButton->OnButtonClickedDelegate.AddDynamic(this, &USMenuMultiplayerWidget::ReturnToMenu);
	}

	if (JoinButton && JoinWidget)
	{
		JoinButton->OnClicked().AddUObject(this, &USMenuMultiplayerWidget::OnJoinButtonClicked);
		JoinWidget->BackButton->OnButtonClickedDelegate.AddDynamic(this, &USMenuMultiplayerWidget::ReturnToMenu);
	}
	
	if (ExitButton)
		ExitButton->OnClicked().AddUObject(this, &USMenuMultiplayerWidget::OnExitButtonClicked);

	JoinWidget->SetVisibility(ESlateVisibility::Collapsed);
	HostWidget->SetVisibility(ESlateVisibility::Collapsed);
}

UWidget* USMenuMultiplayerWidget::NativeGetDesiredFocusTarget() const
{
	return Super::NativeGetDesiredFocusTarget();
}

void USMenuMultiplayerWidget::ReturnToMenu()
{
	MenuOverlay->SetVisibility(ESlateVisibility::Visible);
	
	HostWidget->SetActivated(false);
	JoinWidget->SetActivated(false);
}

void USMenuMultiplayerWidget::OnHostButtonClicked()
{
	if (!HostButton)
		return;

	HostWidget->SetActivated(true);

	JoinWidget->SetActivated(false);
	MenuOverlay->SetVisibility(ESlateVisibility::Collapsed);
}

void USMenuMultiplayerWidget::OnJoinButtonClicked()
{
		if (!JoinButton)
    		return;

	JoinWidget->SetActivated(true);
	
	HostWidget->SetActivated(false);
	MenuOverlay->SetVisibility(ESlateVisibility::Collapsed);
}

void USMenuMultiplayerWidget::OnExitButtonClicked()
{
	if (const UWorld* WorldContext = GetWorld())
	{
		UKismetSystemLibrary::QuitGame(WorldContext, GetOwningPlayer(), EQuitPreference::Quit, false);
	}
}
