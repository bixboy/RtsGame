#include "Framwork/UI/Menu/Multiplayer/SMenuhostSessionWidget.h"

#include "AdvancedSessionsLibrary.h"
#include "CommonTextBlock.h"
#include "CreateSessionCallbackProxyAdvanced.h"
#include "Components/EditableText.h"
#include "Components/Slider.h"
#include "Framwork/Managers/SGameInstanceSubSystem.h"
#include "Framwork/UI/Menu/SButtonBaseWidget.h"


/*---------------- Setup -------------------*/
#pragma region Setup

void USMenuhostSessionWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	InitTextDisplays();

	// Settings
	if (ChangeSessionAccessButton)
		ChangeSessionAccessButton->OnClicked().AddUObject(this, &USMenuhostSessionWidget::OnSessionAccessButtonClicked);
	
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

	if (SessionAccessText)
	{
		FString Text = TEXT("Public");
		SessionAccessText->SetText(FText::FromString(Text));

		Setting3 = ESessionAccess::Public;
		GameSettings.IsPrivateGame = false;
	}

	if (SelectedGameTitleText)
	{
		FString NameText;
		UAdvancedSessionsLibrary::GetPlayerName(GetWorld()->GetFirstPlayerController(), NameText);
		GameSettings.GameName = NameText + " Game";
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
	if (Text.IsEmpty())
	{
		FString NameText;
		UAdvancedSessionsLibrary::GetPlayerName(GetWorld()->GetFirstPlayerController(), NameText);

		GameSettings.GameName = NameText + " Game";
	}
	else
	{
		GameSettings.GameName = Text.ToString();	
	}
}

void USMenuhostSessionWidget::OnLaunchGame()
{
	HostSession();
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

// Session Access
void USMenuhostSessionWidget::OnSessionAccessButtonClicked()
{
	switch (Setting3)
	{
	case ESessionAccess::Public:
		Setting3 = ESessionAccess::Private;
		GameSettings.IsPrivateGame = true;
		break;
		
	case ESessionAccess::Private:
		Setting3 = ESessionAccess::Public;
		GameSettings.IsPrivateGame = false;
		break;
		
	default: ;
	}
	
	if (GameSetting1Text)
	{
		FString Text;
		if (GameSettings.IsPrivateGame)
		{
			Text = TEXT("Private");
		}
		else
		{
			Text = TEXT("Public");
		}
		
		SessionAccessText->SetText(FText::FromString(Text));
	}
}

#pragma endregion

/*---------------- Hosting Session ----------------*/
#pragma region Hosting Session

void USMenuhostSessionWidget::HostSession()
{
	if (auto* Subsys = GetGameInstance()->GetSubsystem<USGameInstanceSubSystem>())
	{
		Subsys->HostSession(GameSettings, LevelLobby);
	}
}

void USMenuhostSessionWidget::OnCreteSessionSuccessGenerateId()
{
	/* FString SessionIdStr = GenerateTruncatedGuid();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, SessionIdStr);
	
	IOnlineSessionPtr SessionInterface = Online::GetSubsystem(GetWorld())->GetSessionInterface();

	if (SessionInterface)
		SessionInterface->CreateSessionIdFromString(SessionIdStr); */
}

FString USMenuhostSessionWidget::GenerateTruncatedGuid()
{
	FGuid Guid = FGuid::NewGuid();

	uint64* Ptr = reinterpret_cast<uint64*>(&Guid);
	uint64 HalfGuid = Ptr[0];

	// Convertit en hex 64 bits => ~16 caractères
	return FString::Printf(TEXT("%016llX"), HalfGuid);
}

#pragma endregion
