#include "Framwork/UI/Menu/GameMenu/SMenuLobbyWidget.h"
#include "EndSessionCallbackProxy.h"
#include "Components/Button.h"
#include "Framwork/SLobbyPlayerController.h"
#include "Framwork/UI/Menu/SButtonBaseWidget.h"
#include "Kismet/GameplayStatics.h"


void USMenuLobbyWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (MenuButton)
		MenuButton->OnButtonClickedDelegate.AddDynamic(this, &USMenuLobbyWidget::OnMenuPressed);

	if (ReadyToggleButton)
		ReadyToggleButton->OnButtonClickedDelegate.AddDynamic(this, &USMenuLobbyWidget::OnReadyPressed);
}

USPlayerLobbyWidget* USMenuLobbyWidget::GetPlayerLobbyWidget()
{
	if (PlayerLobbyWidget)
		return PlayerLobbyWidget;

	return nullptr;
}

void USMenuLobbyWidget::OnMenuPressed()
{
	if (!GetOwningPlayer()->HasAuthority())
	{
		GetWorld()->GetFirstPlayerController()->ClientReturnToMainMenuWithTextReason(FText::FromString("Quit session"));
	}
	else
	{
		DestroySession(GetOwningPlayer());
		UGameplayStatics::OpenLevel(GetWorld(), TEXT("Menu"));
	}
}

void USMenuLobbyWidget::OnReadyPressed()
{
	ASLobbyPlayerController* Player = Cast<ASLobbyPlayerController>(GetWorld()->GetFirstPlayerController());
	if (Player)
	{
		Player->Server_SetPlayerReady();
	}
}

FString USMenuLobbyWidget::GetLevelPath(FSoftObjectPath Level)
{
	FString LevelPath = Level.GetAssetPathString();

	int32 LastDotIndex;
	if (LevelPath.FindLastChar('.', LastDotIndex))
	{
		LevelPath = LevelPath.Left(LastDotIndex) + TEXT("?listen");
	}
	else
	{
		LevelPath += TEXT("?listen");
	}

	return LevelPath;
}
