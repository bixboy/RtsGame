#include "Framwork/UI/Menu/Multiplayer/SMenuhostSessionWidget.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "CommonGameInstance.h"
#include "CommonSessionSubsystem.h"
#include "CommonTextBlock.h"
#include "CommonUserSubsystem.h"
#include "CreateSessionCallbackProxyAdvanced.h"
#include "OnlineSubsystemUtils.h"
#include "PrimaryGameLayout.h"
#include "Components/EditableText.h"
#include "Components/Slider.h"
#include "Framwork/Data/SGameData.h"
#include "Framwork/UI/Menu/SButtonBaseWidget.h"
#include "Framwork/UI/Menu/Multiplayer/SGameDisplayListWidget.h"
#include "Framwork/UI/Menu/Multiplayer/SGameDisplayWidget.h"
#include "Kismet/GameplayStatics.h"

/*---------------- Setup -------------------*/
#pragma region Setup

void USMenuhostSessionWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	NetMode = ECommonSessionOnlineMode::Online;
	InitTextDisplays();

	// Settings
	if (ChangeNetModeButton)
		ChangeNetModeButton->OnClicked().AddUObject(this, &USMenuhostSessionWidget::OnNetWorkModeButtonClicked);
	
	if (ChangeSetting1Button)
		ChangeSetting1Button->OnClicked().AddUObject(this, &USMenuhostSessionWidget::OnSetting1Changed);
	
	if (ChangeSetting2Button)
		ChangeSetting2Button->OnClicked().AddUObject(this, &USMenuhostSessionWidget::OnSetting2Changed);

	if (TimeSlider)
		TimeSlider->OnValueChanged.AddDynamic(this, &USMenuhostSessionWidget::OnSliderChange);

	// Buttons
	if (LaunchButton)
		LaunchButton->OnClicked().AddUObject(this, &USMenuhostSessionWidget::OnLaunchGame);

	if (SelectedGameTitleText)
		SelectedGameTitleText->OnTextCommitted.AddDynamic(this, &USMenuhostSessionWidget::OnGameNameChange);
}

void USMenuhostSessionWidget::InitTextDisplays()
{
	if (NetWorkModeText)
		NetWorkModeText->SetText(UEnum::GetDisplayValueAsText(NetMode));

	if (GameSetting1Text)
	{
		FString Text = FString::FromInt(GameSettings.MaxPlayers) + TEXT(" Players");
		GameSetting1Text->SetText(FText::FromString(Text));
		
		Setting1 = ESettingMaxPlayers::Max2;
	}
	
	if (GameSetting2Text)
	{
		FString Text = FString::FromInt(GameSettings.PlayerLife) + TEXT(" Life");
		GameSetting2Text->SetText(FText::FromString(Text));

		Setting2 = ESettingPlayerHealth::PlayerHealth2;
	}

	if (GameSetting3Text && TimeSlider)
	{
		int Minutes = 2;
		int Seconds = 30;
		GameSetting3Text->SetText(FText::FromString(FString::Printf(TEXT("%d:%02d"), Minutes, Seconds)));

		TimeSlider->SetValue(160);
		GameSettings.RoundDuration = 160;
	}
}

void USMenuhostSessionWidget::SetActivated(const bool bActivate)
{
	if (bActivate)
		SetVisibility(ESlateVisibility::Visible);
	else
		SetVisibility(ESlateVisibility::Collapsed);
}

#pragma endregion

void USMenuhostSessionWidget::OnGameNameChange(const FText& Text, ETextCommit::Type CommitMethod)
{
	GameSettings.GameName = Text.ToString();
}

void USMenuhostSessionWidget::OnLaunchGame()
{
	HostSession();
}

void USMenuhostSessionWidget::OnNetWorkModeButtonClicked()
{
	switch (NetMode)
	{
		case ECommonSessionOnlineMode::Offline:
			NetMode = ECommonSessionOnlineMode::LAN;
			break;
		case ECommonSessionOnlineMode::LAN:
			NetMode = ECommonSessionOnlineMode::Online;
			break;
		case ECommonSessionOnlineMode::Online:
			NetMode = ECommonSessionOnlineMode::Offline;
			break;
		default: NetMode = ECommonSessionOnlineMode::Offline;
	}

	if (NetWorkModeText)
	{
		NetWorkModeText->SetText(UEnum::GetDisplayValueAsText(NetMode));
	}
}

/*---------------- Settings ------------------------*/
#pragma region Settings

// Setting Max Player
void USMenuhostSessionWidget::OnSetting1Changed()
{
	switch (Setting1)
	{
	case ESettingMaxPlayers::Max1:
		Setting1 = ESettingMaxPlayers::Max2;
		GameSettings.MaxPlayers = 2;
		break;
		
	case ESettingMaxPlayers::Max2:
		Setting1 = ESettingMaxPlayers::Max3;
		GameSettings.MaxPlayers = 3;
		break;
		
	case ESettingMaxPlayers::Max3:
		Setting1 = ESettingMaxPlayers::Max1;
		GameSettings.MaxPlayers = 4;
		break;
		
	default: ;
	}
	
	if (GameSetting1Text)
	{
		FString Text = FString::FromInt(GameSettings.MaxPlayers) + TEXT(" Players");
		GameSetting1Text->SetText(FText::FromString(Text));
	}
}

// Setting Lifes
void USMenuhostSessionWidget::OnSetting2Changed()
{
	switch (Setting2)
	{
		case ESettingPlayerHealth::PlayerHealth1:
			Setting2 = ESettingPlayerHealth::PlayerHealth2;
			GameSettings.PlayerLife = 2;
			break;
		
		case ESettingPlayerHealth::PlayerHealth2:
			Setting2 = ESettingPlayerHealth::PlayerHealth3;
			GameSettings.PlayerLife = 3;
			break;
		
		case ESettingPlayerHealth::PlayerHealth3:
			Setting2 = ESettingPlayerHealth::PlayerHealth4;
			GameSettings.PlayerLife = 4;
			break;
		
		case ESettingPlayerHealth::PlayerHealth4:
			Setting2 = ESettingPlayerHealth::PlayerHealth5;
			GameSettings.PlayerLife = 5;
			break;
		
		case ESettingPlayerHealth::PlayerHealth5:
			Setting2 = ESettingPlayerHealth::PlayerHealth6;
			GameSettings.PlayerLife = 6;
			break;
		
		case ESettingPlayerHealth::PlayerHealth6:
			Setting2 = ESettingPlayerHealth::PlayerHealth7;
			GameSettings.PlayerLife = 7;
			break;
		
		case ESettingPlayerHealth::PlayerHealth7:
			Setting2 = ESettingPlayerHealth::PlayerHealth1;
			GameSettings.PlayerLife = 8;
			break;
	default: ;
	}

	if (GameSetting2Text)
	{
		FString Text = FString::FromInt(GameSettings.PlayerLife) + TEXT(" Life");
		GameSetting2Text->SetText(FText::FromString(Text));
	}
}

// Time Slider
void USMenuhostSessionWidget::OnSliderChange(float Value)
{
	 int RoundedValue = FMath::RoundToInt(Value / 30.0f) * 30;
    
    // Mettre à jour la durée du round
    GameSettings.RoundDuration = RoundedValue;

    // Calculer les minutes et secondes
    int Minutes = RoundedValue / 60;
    int Seconds = RoundedValue % 60;
    
    // Mettre à jour l'affichage (supposons que vous ayez un UTextBlock* nommé DurationText)
    if (GameSetting3Text)
    {
        GameSetting3Text->SetText(FText::FromString(FString::Printf(TEXT("%d:%02d"), Minutes, Seconds)));
    }
}

#pragma endregion

/*---------------- Hosting Session ----------------*/
#pragma region Hosting Session

void USMenuhostSessionWidget::HostSession()
{
    UE_LOG(LogTemp, Log, TEXT("HostSession: Début de la fonction."));
	
    UWorld* World = GetWorld();
	APlayerController* PlayerController = World->GetFirstPlayerController();
	
    TArray<FSessionPropertyKeyPair> SessionProperties;
    SessionProperties.Add(FSessionPropertyKeyPair(TEXT("GameName"), GameSettings.GameName));
    SessionProperties.Add(FSessionPropertyKeyPair(TEXT("MapName"), GameSettings.MapName));
	
    // Créer la session
    UCreateSessionCallbackProxyAdvanced* CreateSessionProxy = UCreateSessionCallbackProxyAdvanced::CreateAdvancedSession(
        World,
        SessionProperties,
        PlayerController,
        GameSettings.MaxPlayers,
        0,
        false,
		true,
		false,
		true,
		true,
		true,
		false,
		false,
		false,
		true,
		false,
		true
    );

    if (CreateSessionProxy)
    {
        UE_LOG(LogTemp, Log, TEXT("HostSession: CreateSessionProxy créé avec succès."));

        CreateSessionProxy->OnSuccess.AddDynamic(this, &USMenuhostSessionWidget::OnCreateSessionSuccess);
        CreateSessionProxy->OnFailure.AddDynamic(this, &USMenuhostSessionWidget::OnCreateSessionFailure);

    	CreateSessionProxy->Activate();

    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("HostSession: Échec de la création du CreateSessionProxy."));
    }
	
}

void USMenuhostSessionWidget::OnCreateSessionFailure()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Session creation failed"));
	UE_LOG(LogTemp, Error, TEXT("Session creation failed"));
}

#pragma endregion
