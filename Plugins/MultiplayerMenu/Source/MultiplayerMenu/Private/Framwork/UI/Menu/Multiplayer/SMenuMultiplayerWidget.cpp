#include "Framwork/UI/Menu/Multiplayer/SMenuMultiplayerWidget.h"
#include "PrimaryGameLayout.h"
#include "Framwork/Data/StaticGameData.h"
#include "Framwork/UI/Menu/SButtonBaseWidget.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Framwork/UI/Menu/Multiplayer/SMenuhostSessionWidget.h"
#include "Framwork/UI/Menu/Multiplayer/SMenuJoinSessionWidget.h"

void USMenuMultiplayerWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (HostButton)
		HostButton->OnClicked().AddUObject(this, &USMenuMultiplayerWidget::OnHostButtonClicked);

	if (JoinButton)
		JoinButton->OnClicked().AddUObject(this, &USMenuMultiplayerWidget::OnJoinButtonClicked);
	
	if (ExitButton)
		ExitButton->OnClicked().AddUObject(this, &USMenuMultiplayerWidget::OnExitButtonClicked);
}

UWidget* USMenuMultiplayerWidget::NativeGetDesiredFocusTarget() const
{
	return Super::NativeGetDesiredFocusTarget();
	
}

void USMenuMultiplayerWidget::OnHostButtonClicked()
{
	if (!HostWidgetClass.IsValid())
		return;

	if (const UWorld* WorldContext = GetWorld())
	{
		if (UPrimaryGameLayout* RootLayout = UPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(WorldContext))
		{
			RootLayout->PushWidgetToLayerStackAsync<UCommonActivatableWidget>(UILayerTags::TAG_UI_LAYER_MENU, false, HostWidgetClass,
				[this](EAsyncWidgetLayerState State, UCommonActivatableWidget* Screen)
				{
					switch (State)
					{
						case EAsyncWidgetLayerState::AfterPush:
							return;
						case EAsyncWidgetLayerState::Canceled:
							return;
					}
				});
		}
	}
}

void USMenuMultiplayerWidget::OnJoinButtonClicked()
{
	if (!JoinWidgetClass.IsValid())
		return;

	if (const UWorld* WorldContext = GetWorld())
	{
		if (UPrimaryGameLayout* RootLayout = UPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(WorldContext))
		{
			RootLayout->PushWidgetToLayerStackAsync<UCommonActivatableWidget>(UILayerTags::TAG_UI_LAYER_MENU, false, JoinWidgetClass,
				[this](EAsyncWidgetLayerState State, UCommonActivatableWidget* Screen)
				{
					switch (State)
					{
						case EAsyncWidgetLayerState::AfterPush:
							return;
						case EAsyncWidgetLayerState::Canceled:
							return;
					}
				});
		}
	}
}

void USMenuMultiplayerWidget::OnExitButtonClicked()
{
	if (const UWorld* WorldContext = GetWorld())
	{
		UKismetSystemLibrary::QuitGame(WorldContext, GetOwningPlayer(), EQuitPreference::Quit, false);
	}
}
