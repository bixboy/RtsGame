#include "Manager/GameModeManager.h"
#include "Framwork/Managers/SGameInstanceSubSystem.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"


// ========== Setup ========= //
#pragma region Setup

void AGameModeManager::StartPlay()
{
	Super::StartPlay();

	if (HasAuthority())
	{
		if (USGameInstanceSubSystem* Instance = GetWorld()->GetSubsystem<USGameInstanceSubSystem>())
		{
			GameSettings = Instance->GetGameSettings();
		}
		
		BeginMatch();
	}
}

void AGameModeManager::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (NewPlayer)
	{
		const int32 AssignedTeam = NextTeamId++;
		PlayersTeam.Add(AssignedTeam, NewPlayer);
	}
}

#pragma endregion


// ========== Match ========= //
void AGameModeManager::BeginMatch()
{
	MatchState = EMatchState::InProgress;
	GameStartTime = GetWorld()->GetTimeSeconds();
}

void AGameModeManager::CheckForVictory()
{
	if (!HasAuthority()) return;

	if (GetElapsedTime() >= 180.f)
	{
		EndMatch();
	}
}

void AGameModeManager::EndMatch()
{
	MatchState = EMatchState::Completed;
	
	for (auto It = GameState->PlayerArray.CreateIterator(); It; ++It)
	{
		if (APlayerController* PC = Cast<APlayerController>((*It)->GetOwner()))
		{
			PC->ClientMessage(TEXT("Match Completed!"));
		}
	}
}


// ------- Getter

float AGameModeManager::GetElapsedTime() const
{
	return GetWorld()->GetTimeSeconds() - GameStartTime;
}
