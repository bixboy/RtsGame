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
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(CheckSessionTimerHandle);
	}
	Super::Deinitialize();
}

void USGameInstanceSubSystem::NetworkFailureHappened(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type Arg,
                                                     const FString& String)
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
	if (UWorld* World = GetWorld())
	{
		if (World->GetNumPlayerControllers() == 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("La session est vide, on quitte."));
			LeaveSession();
		}
	}
}

void USGameInstanceSubSystem::LeaveSession()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Menu"), true);
	DestroySession(GetWorld(), GetWorld()->GetFirstPlayerController());
}
