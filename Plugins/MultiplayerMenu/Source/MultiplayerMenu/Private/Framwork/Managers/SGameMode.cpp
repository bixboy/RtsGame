#include "Framwork/Managers/SGameMode.h"

#include "Engine/AssetManager.h"
#include "Framwork/SPlayerController.h"
#include "Framwork/Data/SGameData.h"
#include "Framwork/Data/StaticGameData.h"
#include "Framwork/Managers/SGameState.h"
#include "Kismet/GameplayStatics.h"

ASGameMode::ASGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bGameReady = false;
}

void ASGameMode::InitGameState()
{
	Super::InitGameState();
	SessionPlayers.Empty();
	
	if(ASGameState* SGameState = Cast<ASGameState>(GameState))
	{
		SGameState->OnGameStateReady.AddDynamic(this, &ASGameMode::OnGameReady);
	}

	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ASGameMode::HandleGameDataAssignment);
}

void ASGameMode::GenericPlayerInitialization(AController* C)
{
	Super::GenericPlayerInitialization(C);

	if (ASPlayerController* Player = Cast<ASPlayerController>(C))
	{
		SessionPlayers.AddUnique(Player);
	}
}

void ASGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	if (IsGameDataLoaded())
	{
		Super::HandleStartingNewPlayer_Implementation(NewPlayer);	
	}
}

bool ASGameMode::PlayerCanRestart_Implementation(APlayerController* Player)
{
	if (bGameReady)
	{
		return Super::PlayerCanRestart_Implementation(Player);	
	}

	return false;
}

void ASGameMode::FailedToRestartPlayer(AController* NewPlayer)
{
	if (bGameReady)
	{
		if (APlayerController* NewPC = Cast<APlayerController>(NewPlayer))
		{
			if (PlayerCanRestart(NewPC))
			{
				RequestPlayerRestartNextFrame(NewPlayer, false);
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("FailedToRestartPlayer(%s) and PlayerCanRestart returned false, so we're not going to try again"), *GetPathNameSafe(NewPlayer));
			}
		}
	}
	else
	{
		RequestPlayerRestartNextFrame(NewPlayer, false);
	}
}

void ASGameMode::RequestPlayerRestartNextFrame(AController* Controller, bool bForceReset)
{
	if (bForceReset && Controller)
	{
		Controller->Reset();
	}
	
	if (APlayerController* Player = Cast<APlayerController>(Controller))
	{
		GetWorldTimerManager().SetTimerForNextTick(Player, &APlayerController::ServerRestartPlayer_Implementation);
	}
}

void ASGameMode::HandleGameDataAssignment()
{
	if (!CurrentGameDataAssetId.IsValid() && UGameplayStatics::HasOption(OptionsString, S_MP_SETTINGS_GAMEMODE))
	{
		const FString GameDataFromOptions = UGameplayStatics::ParseOption(OptionsString, S_MP_SETTINGS_GAMEMODE);
		CurrentGameDataAssetId = FPrimaryAssetId(FPrimaryAssetType(USGameData::StaticClass()->GetFName()), FName(*GameDataFromOptions));
	}

	if(!CurrentGameDataAssetId.IsValid())
	{
		CurrentGameDataAssetId = FPrimaryAssetId(FPrimaryAssetType(S_DATA_ASSET_TYPE_GAMEDATA), S_DATA_ASSET_TYPE_GAMEDATA_DEFAULT);
	}

	if(CurrentGameDataAssetId.IsValid())
	{
		OnGameDataAssignment();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[ASGameMode::HandleGameDataAssignment] Invalid GameDataAssetId"));
	}
}

void ASGameMode::OnGameDataAssignment()
{
	if (UAssetManager* AssetManager = UAssetManager::GetIfInitialized())
	{
		if (CurrentGameDataAssetId.IsValid())
		{
			TArray<FName> Bundles;
			Bundles.Add(S_DATA_ASSET_BUNDLE_GAME);
			const FStreamableDelegate GroupDataLoadedDelegate = FStreamableDelegate::CreateUObject(this, &ASGameMode::OnGameDataLoaded);
			AssetManager->LoadPrimaryAsset(CurrentGameDataAssetId, Bundles, GroupDataLoadedDelegate);
		}
	}
}

void ASGameMode::OnGameDataLoaded()
{
	if (GameState && CurrentGameDataAssetId.IsValid())
	{
		if (ASGameState* SGameState = Cast<ASGameState>(GameState))
		{
			if (USGameData* GameData = GetGameData())
				SGameState->SetGameData(GameData);
		}
	}
}

USGameData* ASGameMode::GetGameData() const
{
	if (CurrentGameDataAssetId.IsValid())
	{
		if (const UAssetManager* AssetManager = UAssetManager::GetIfInitialized())
		{
			return Cast<USGameData>(AssetManager->GetPrimaryAssetObject(CurrentGameDataAssetId));
		}
	}

	return nullptr;
}

void ASGameMode::OnGameReady()
{
	bGameReady = true;
	InitialisePlayer();
}

void ASGameMode::InitialisePlayer()
{
	// Init Player Data
	for (int i = 0; i < SessionPlayers.Num(); i++)
	{
		if ((SessionPlayers[i]) && (!SessionPlayers[i]->GetPawn()))
		{
			if (const USGameData* GameData = GetGameData())
			{
				if (PlayerCanRestart(SessionPlayers[i]))
				{
					RestartPlayer(SessionPlayers[i]);
				}
			}
		}
	}
}
