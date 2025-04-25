#include "Framwork/SPlayerController.h"
#include "Framwork/UI/Menu/Multiplayer/SMenuMultiplayerWidget.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineSessionInterface.h"

ASPlayerController::ASPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void ASPlayerController::BeginPlay()
{
	Super::BeginPlay();

	IOnlineSubsystem* Subsys = Online::GetSubsystem(GetWorld());
	if (Subsys)
	{
		IOnlineSessionPtr Sessions = Subsys->GetSessionInterface();
		if (Sessions.IsValid())
		{
			FName SessionName = NAME_GameSession; 
			FNamedOnlineSession* Existing = Sessions->GetNamedSession(SessionName);
            
			if (Existing && Existing->SessionState != EOnlineSessionState::NoSession)
			{
				Sessions->DestroySession(SessionName);
			}
		}
	}
	
	if (MenuWidgetClass)
	{
		ShowMenu(MenuWidgetClass);

		FInputModeUIOnly InputMode;
		SetInputMode(InputMode);
		bShowMouseCursor = true;
	}
}


void ASPlayerController::ShowMenu(const TSubclassOf<UCommonActivatableWidget> MenuClass)
{
	if (MenuClass == nullptr)
		return;
    
	UWorld* WorldContext = GetWorld();
	if (!WorldContext)
		return;
	
	// Crée le widget en utilisant le PlayerController comme owning object
	UCommonActivatableWidget* MenuWidget = CreateWidget<UCommonActivatableWidget>(this, MenuClass.Get());
	if (MenuWidget)
	{
		MenuWidget->AddToViewport();
		UE_LOG(LogTemp, Warning, TEXT("Menu successfully added to viewport"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Failed to add menu to viewport"));
		UE_LOG(LogTemp, Warning, TEXT("Failed to create menu widget"));
	}
}

void ASPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (HasAuthority()) // Assurez-vous que le code s'exécute uniquement sur le serveur
	{
		IOnlineSessionPtr Sessions = Online::GetSubsystem(GetWorld())->GetSessionInterface();
		if (Sessions.IsValid())
		{
			FName SessionName(TEXT("Session1"));
			FNamedOnlineSession* Session = Sessions->GetNamedSession(SessionName);
			if (Session && Session->RegisteredPlayers.Num() == 0)
			{
				Sessions->DestroySession(SessionName);
			}
		}
	}
}
