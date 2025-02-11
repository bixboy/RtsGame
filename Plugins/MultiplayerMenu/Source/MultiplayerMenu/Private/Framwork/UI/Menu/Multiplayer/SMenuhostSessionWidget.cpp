#include "Framwork/UI/Menu/Multiplayer/SMenuhostSessionWidget.h"

#include "CommonGameInstance.h"
#include "CommonSessionSubsystem.h"
#include "CommonTextBlock.h"
#include "CommonUserSubsystem.h"
#include "PrimaryGameLayout.h"
#include "Framwork/Data/SGameData.h"
#include "Framwork/UI/Menu/SButtonBaseWidget.h"
#include "Framwork/UI/Menu/Multiplayer/SGameDisplayListWidget.h"
#include "Framwork/UI/Menu/Multiplayer/SGameDisplayWidget.h"

void USMenuhostSessionWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	NetMode = ECommonSessionOnlineMode::Online;
	InitTextDisplays();

	if (ChangeNetModeButton)
	{
		ChangeNetModeButton->OnClicked().AddUObject(this, &USMenuhostSessionWidget::OnNetWorkModeButtonClicked);
	}
	
	if (ChangeSetting1Button)
		ChangeSetting1Button->OnClicked().AddUObject(this, &USMenuhostSessionWidget::OnSetting1Changed);
	if (ChangeSetting2Button)
		ChangeSetting2Button->OnClicked().AddUObject(this, &USMenuhostSessionWidget::OnSetting2Changed);

	if (LaunchButton)
		LaunchButton->OnClicked().AddUObject(this, &USMenuhostSessionWidget::OnlaunchGame);
	if (BackButton)
		BackButton->OnClicked().AddUObject(this, &USMenuhostSessionWidget::OnBackGame);

	if (GameList)
	{
		GameList->OnGameListCreated.AddDynamic(this, &USMenuhostSessionWidget::OnGameListCreated);
		GameList->OnGameSelected.AddDynamic(this, &USMenuhostSessionWidget::OnGameSelected);
	}
}

void USMenuhostSessionWidget::InitTextDisplays() const
{
	if (NetWorkModeText)
		NetWorkModeText->SetText(UEnum::GetDisplayValueAsText(NetMode));

	if (GameSetting1Text)
		GameSetting1Text->SetText(UEnum::GetDisplayValueAsText(Setting1));
	if (GameSetting2Text)
		GameSetting1Text->SetText(UEnum::GetDisplayValueAsText(Setting2));
}

void USMenuhostSessionWidget::AttenptOnlineLogin()
{
	if (!GetOwningPlayer() || !GetOwningPlayer()->GetWorld())
		return;

	if (const UCommonGameInstance* GameInstance = Cast<UCommonGameInstance>(GetOwningPlayer()->GetWorld()->GetGameInstance()))
	{
		if (UCommonUserSubsystem* UserSubsystem = GameInstance->GetSubsystem<UCommonUserSubsystem>())
		{
			UserSubsystem->OnUserInitializeComplete.AddDynamic(this, &USMenuhostSessionWidget::OnUserOnlineLogin);
			UserSubsystem->TryToLoginForOnlinePlay(0);
		}
	}
}

void USMenuhostSessionWidget::OnUserOnlineLogin(const UCommonUserInfo* UserInfo, bool bSuccess, FText Error,
	ECommonUserPrivilege RequestedPrivilege, ECommonUserOnlineContext OnlineContext)
{
	if (bSuccess)
	{
		HostSession();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed To Login Online"))
	}
}

void USMenuhostSessionWidget::HostSession()
{
	if (!GetOwningPlayer() || !GetOwningPlayer()->GetWorld())
		return;

	if (const UCommonGameInstance* GameInstance = Cast<UCommonGameInstance>(GetOwningPlayer()->GetWorld()->GetGameInstance()))
	{
		if (UCommonSessionSubsystem* SessionSubsystem = GameInstance->GetSubsystem<UCommonSessionSubsystem>())
		{
			SessionSubsystem->OnCreateSessionCompleteEvent.AddUObject(this, &USMenuhostSessionWidget::OnSessionCreated);
			UCommonSession_HostSessionRequest* Request = CreateHostingRequest();
			SessionSubsystem->HostSession(GetOwningPlayer(), Request);
		}
	}
}

UCommonSession_HostSessionRequest* USMenuhostSessionWidget::CreateHostingRequest() const
{
	UCommonSession_HostSessionRequest* Request = NewObject<UCommonSession_HostSessionRequest>();

	if (GameDataId.IsValid())
	{
		if (const UAssetManager* AssetManager = UAssetManager::GetIfInitialized())
		{
			if (const USGameData* GameData = Cast<USGameData>(AssetManager->GetPrimaryAssetObject(GameDataId)))
			{
				const FString GameDataName = GameDataId.PrimaryAssetName.ToString();
				Request->ModeNameForAdvertisement = GameDataName;
				Request->OnlineMode = ECommonSessionOnlineMode::Online;
				Request->bUseLobbies = true;
				Request->MapID = GameData->MapID;
				Request->ExtraArgs = GameData->ExtraArgs;
				Request->MaxPlayerCount = GameData->MaxPlayerCount;
			}
		}
	}

	return Request;
}

void USMenuhostSessionWidget::OnSessionCreated(const FOnlineResultInformation& Result)
{
	if (Result.bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Session Created"))
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to Create Session"))
	}
}

void USMenuhostSessionWidget::OnlaunchGame()
{
	AttenptOnlineLogin();
}

void USMenuhostSessionWidget::OnBackGame()
{
	if (const UWorld* World = GetWorld())
	{
		if (UPrimaryGameLayout* RootLayout = UPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(World))
		{
			RootLayout->FindAndRemoveWidgetFromLayer(this);
		}
	}
}

void USMenuhostSessionWidget::OnSetting1Changed()
{
	switch (Setting1)
	{
		case ESetting1::Set1:
			Setting1 = ESetting1::Set2;
			break;
		case ESetting1::Set2:
			Setting1 = ESetting1::Set1;
			break;
		default: ;
	}

	if (GameSetting1Text)
		GameSetting1Text->SetText(UEnum::GetDisplayValueAsText(Setting1));
}

void USMenuhostSessionWidget::OnSetting2Changed()
{
	switch (Setting2)
	{
	case ESetting2::Set3:
		Setting2 = ESetting2::Set4;
		break;
	case ESetting2::Set4:
		Setting2 = ESetting2::Set3;
		break;
	default: ;
	}

	if (GameSetting2Text)
		GameSetting2Text->SetText(UEnum::GetDisplayValueAsText(Setting2));
}

void USMenuhostSessionWidget::OnNetWorkModeButtonClicked()
{
	switch (NetMode)
	{
		case ECommonSessionOnlineMode::Offline:
			NetMode = ECommonSessionOnlineMode::LAN;
			return;
		case ECommonSessionOnlineMode::LAN:
			NetMode = ECommonSessionOnlineMode::Online;
			return;
		case ECommonSessionOnlineMode::Online:
			NetMode = ECommonSessionOnlineMode::Offline;
			return;
		default: NetMode = ECommonSessionOnlineMode::LAN;
	}

	if (NetWorkModeText)
	{
		NetWorkModeText->SetText(UEnum::GetDisplayValueAsText(NetMode));
	}
}



void USMenuhostSessionWidget::OnGameListCreated()
{
	TArray<UWidget*> DisplayWidgets = GameList->GetGameListWidgetList();
	if (DisplayWidgets.IsValidIndex(0))
	{
		if (const USGameDisplayWidget* DisplayWidget = Cast<USGameDisplayWidget>(DisplayWidgets[0]))
		{
			const FPrimaryAssetId& DisplayGameData = DisplayWidget->GetGameDataAsset();
			OnGameSelected(DisplayGameData);
		}
	}
}

void USMenuhostSessionWidget::OnGameSelected(const FPrimaryAssetId& SelectedGameData)
{
	if (!SelectedGameData.IsValid())
		return;

	if (const UAssetManager* AssetManager = UAssetManager::GetIfInitialized())
	{
		if (const USGameData* GameData = Cast<USGameData>(AssetManager->GetPrimaryAssetObject(SelectedGameData)))
		{
			if (SelectedGameTitleText && GameData)
				SelectedGameTitleText->SetText(GameData->GameName);
		}
	}
}
