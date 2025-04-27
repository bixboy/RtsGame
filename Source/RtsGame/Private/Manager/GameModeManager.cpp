#include "Manager/GameModeManager.h"
#include "Framwork/Managers/SGameInstanceSubSystem.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Player/RtsPlayerController.h"


// ========== Setup ========= //
#pragma region Setup

void AGameModeManager::StartPlay()
{
	Super::StartPlay();

	if (HasAuthority())
	{
		if (APlayerController* LocalPC = UGameplayStatics::GetPlayerController(this, 0))
		{
			bool bAlready = false;
			for (auto& Pair : PlayersTeam)
			{
				if (Pair.Value == LocalPC)
				{
					bAlready = true;
					break;
				}
			}
			if (!bAlready)
			{
				AssignTeamToPlayer(LocalPC);
			}
		}

		if (UGameInstance* GI = GetWorld()->GetGameInstance())
		{
			if (USGameInstanceSubSystem* SubSystem = GI->GetSubsystem<USGameInstanceSubSystem>())
			{
				GameSettings = SubSystem->GetGameSettings();
			}
		}
		
		BeginMatch();
	}
}

void AGameModeManager::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (!HasAuthority() || !NewPlayer)
		return;
	
	AssignTeamToPlayer(NewPlayer);
}

void AGameModeManager::AssignTeamToPlayer(APlayerController* PC)
{
	if (!PC) return;

	const int32 AssignedTeam = NextTeamId++;
	PlayersTeam.Add(AssignedTeam, PC);

	if (ARtsPlayerController* RtsPC = Cast<ARtsPlayerController>(PC))
	{
		RtsPC->SetPlayerTeam(AssignedTeam);
	}

	UE_LOG(LogTemp, Warning, TEXT("Assigned %s to team %d"), *PC->GetName(), AssignedTeam);
}

#pragma endregion


// ========== Match ========= //
void AGameModeManager::BeginMatch()
{
	GameMatchState = EMatchState::InProgress;
	GameStartTime = GetWorld()->GetTimeSeconds();
}

void AGameModeManager::CheckForVictory()
{
	if (!HasAuthority()) return;

	if (GetElapsedTime() >= 180.f)
	{
		EndGameMatch();
	}
}

void AGameModeManager::EndGameMatch()
{
	GameMatchState = EMatchState::Completed;
	
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

int AGameModeManager::GetPlayerTeam(APlayerController* Player)
{
	if (!Player) return -1;

	if (const int32* FoundTeam = PlayersTeam.FindKey(Player))
	{
		return *FoundTeam;
	}

	return -1;
}

APlayerController* AGameModeManager::GetPlayerByTeam(int TeamNum)
{
	if (APlayerController** FoundPC = PlayersTeam.Find(TeamNum))
	{
		return *FoundPC;
	}

	return nullptr;
}
