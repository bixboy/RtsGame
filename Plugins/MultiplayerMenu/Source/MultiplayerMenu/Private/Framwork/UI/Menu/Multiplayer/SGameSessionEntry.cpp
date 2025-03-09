#include "Framwork/UI/Menu/Multiplayer/SGameSessionEntry.h"
#include "AdvancedSessionsLibrary.h"
#include "Framwork/Data/BlueprintSessionResultObject.h"
#include "Framwork/UI/Menu/Multiplayer/SGameSessionButton.h"

void USGameSessionEntry::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (SessionButton)
		SessionButton->OnClicked().AddUObject(this, &USGameSessionEntry::OnSessionSelected);
}

void USGameSessionEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	if (!ListItemObject)
	{
		UE_LOG(LogTemp, Error, TEXT("ListItemObject est nul."));
		return;
	}

	if (UBlueprintSessionResultObject* SessionResult = Cast<UBlueprintSessionResultObject>(ListItemObject))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Session Search Result"));
	
		SessionSearchResult = SessionResult->SessionResult;
		OnGameDataLoaded();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Échec du cast de ListItemObject en FBlueprintSessionResult."));
	}
}

void USGameSessionEntry::OnSessionSelected()
{
	JoinSession(SessionSearchResult);
}

void USGameSessionEntry::JoinSession_Implementation(FBlueprintSessionResult Session)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Start Join Session"));
}

void USGameSessionEntry::OnGameDataLoaded()
{
	// Récupérer les paramètres supplémentaires de la session
	TArray<FSessionPropertyKeyPair> ExtraSettings;
	UAdvancedSessionsLibrary::GetExtraSettings(SessionSearchResult, ExtraSettings);

	FString GameName;
	FString MapName;

	// Parcourir les paramètres pour trouver "GameName" et "MapName"
	for (const FSessionPropertyKeyPair& Setting : ExtraSettings)
	{
		if (Setting.Key == "GameName")
		{
			GameName = Setting.Data.ToString();
		}
		else if (Setting.Key == "MapName")
		{
			MapName = Setting.Data.ToString();
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("GameName: %s, MapName: %s"), *GameName, *MapName));

	// Mettre à jour les informations du bouton de session
	if (SessionButton)
	{
		SessionButton->SetTextDisplays(
			FText::FromString(GameName),
			FText::FromString(MapName),
			FText::AsNumber(SessionSearchResult.OnlineResult.PingInMs),
			FText::AsNumber(SessionSearchResult.OnlineResult.Session.SessionSettings.NumPublicConnections - SessionSearchResult.OnlineResult.Session.NumOpenPublicConnections),
			FText::AsNumber(SessionSearchResult.OnlineResult.Session.SessionSettings.NumPublicConnections)
		);
	}
}
