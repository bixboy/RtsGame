#include "Framwork/Managers/SGameInstanceSubSystem.h"
#include "Kismet/GameplayStatics.h"

void USGameInstanceSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	if (GEngine)
	{
		GEngine->OnNetworkFailure().AddUObject(this, &USGameInstanceSubSystem::NetworkFailureHappened);
		GEngine->TravelFailureEvent.AddUObject(this, &USGameInstanceSubSystem::TravelFailureHappened);
	}

	if (GetWorld())
		GetWorld()->GetTimerManager().SetTimer(CheckSessionTimerHandle, this, &USGameInstanceSubSystem::CheckSessionEmpty, 5.0f, true);
}

void USGameInstanceSubSystem::Deinitialize()
{
	if (GEngine)
	{
		GEngine->OnNetworkFailure().RemoveAll(this);
		GEngine->TravelFailureEvent.RemoveAll(this);
	}
	
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(CheckSessionTimerHandle);
	}
	
	Super::Deinitialize();
}

void USGameInstanceSubSystem::NetworkFailureHappened(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type Arg, const FString& String)
{
	if (Arg == ENetworkFailure::ConnectionLost || Arg == ENetworkFailure::ConnectionTimeout)
	{
		LeaveSession();
	}
}

void USGameInstanceSubSystem::TravelFailureHappened(UWorld* World, ETravelFailure::Type Arg, const FString& String)
{
	
}

void USGameInstanceSubSystem::CheckSessionEmpty()
{
	if (GetWorld()->GetNumPlayerControllers() == 0)
	{
		LeaveSession();
	}
}

FGameSettings USGameInstanceSubSystem::GetGameSettings()
{
	return PendingSettings;
}

void USGameInstanceSubSystem::LeaveSession()
{
	DestroySession(GetWorld(), GetGameInstance()->GetFirstLocalPlayerController());
}

void USGameInstanceSubSystem::OnDestroySessionComplete()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Menu"), true);
}



// ============= Session Hosting ============= //
void USGameInstanceSubSystem::HostSession(const FGameSettings Settings, TSoftObjectPtr<UWorld> LevelToOpen)
{
	PendingSettings = Settings;
	LevelLobby = LevelToOpen;

	UWorld* World = GetWorld();
	APlayerController* PlayerController = World->GetFirstPlayerController();

	TArray<FSessionPropertyKeyPair> SessionProperties;
	SessionProperties.Add(FSessionPropertyKeyPair(TEXT("GameName"), PendingSettings.GameName));
	SessionProperties.Add(FSessionPropertyKeyPair(TEXT("MapName"), PendingSettings.MapName));
	SessionProperties.Add(FSessionPropertyKeyPair(TEXT("SessionAccess"), PendingSettings.IsPrivateGame ? 1 : 0));

	CreateProxy = UCreateSessionCallbackProxyAdvanced::CreateAdvancedSession(
		World,
		SessionProperties,
		PlayerController,
		PendingSettings.MaxPlayers,
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

	if (CreateProxy)
	{
		UE_LOG(LogTemp, Log, TEXT("HostSession: CreateSessionProxy créé avec succès."));

		CreateProxy->OnSuccess.AddDynamic(this, &USGameInstanceSubSystem::HandleHostSuccess);
		CreateProxy->OnFailure.AddDynamic(this, &USGameInstanceSubSystem::HandleHostFailure);

		CreateProxy->Activate();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HostSession: Échec de la création du CreateSessionProxy."));
	}
}

void USGameInstanceSubSystem::HandleHostSuccess()
{
	FSoftObjectPath Path = LevelLobby.ToSoftObjectPath();
	FString Level = GetLevelPath(Path);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Level);
	
	GetWorld()->ServerTravel(Level, true);
}

void USGameInstanceSubSystem::HandleHostFailure()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Session creation failed"));
	UE_LOG(LogTemp, Error, TEXT("Session creation failed"));
}

FString USGameInstanceSubSystem::GetLevelPath(FSoftObjectPath Level)
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
