#include "Framwork/Components/SChatComponent.h"

#include "AdvancedSessionsLibrary.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Framwork/UI/Menu/GameMenu/SChatBoxWidget.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Framwork/UI/Menu/GameMenu/SChatWidget.h"


void USChatComponent::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* Controller = Cast<APlayerController>(GetOwner());
	if (Controller && Controller->IsLocalPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(Controller->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MappingContext, 0);
			SetupInputComponent();
		}
		
		ChatWidget = CreateWidget<USChatWidget>(Controller, ChatWidgetClass);
		if (ChatWidget)
		{
			ChatWidget->AddToViewport();
			ChatBox = ChatWidget->ChatBox;	
		}
	}
}

void USChatComponent::SetupInputComponent()
{
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(Cast<APlayerController>(GetOwner())->InputComponent))
	{
		EnhancedInput->BindAction(ScrollAction, ETriggerEvent::Started, this, &USChatComponent::OnScrolling);
		EnhancedInput->BindAction(EnterAction, ETriggerEvent::Started, this, &USChatComponent::OnEnterPressed);
	}
}

void USChatComponent::Server_SendMessage_Implementation(const FString& NewMessage)
{
	if (CommandCheck(NewMessage)) return;
	
	TArray<APlayerState*> Players;
	Players = GetWorld()->GetGameState()->PlayerArray;
	
	for (APlayerState* Player : Players)
	{
		USChatComponent* ChatComponent = Player->GetPlayerController()->GetComponentByClass<USChatComponent>();
		ChatComponent->Client_AddMessage(NewMessage, true);
	}
}

void USChatComponent::Client_AddMessage_Implementation(const FString& NewMessage, const bool bTextToSpeech)
{
	ChatBox->AddChatMessage(NewMessage, bTextToSpeech);
}

void USChatComponent::OnScrolling(const FInputActionValue& InputActionValue)
{
	float InputValue = InputActionValue.Get<float>();
	
	ChatBox->Scroll(InputValue > 0);
}

void USChatComponent::OnEnterPressed()
{
	ChatBox->FocusOnChat();
}

bool USChatComponent::CommandCheck(FString Message)
{
    FString PlayerName;
    FString CommandText;
    if (Message.Split(TEXT(":"), &PlayerName, &CommandText))
		 CommandText.RemoveAt(0);
	
    FString NewMessage;

    if (CommandText.Len() > 0 && CommandText[0] == '/')
    {
        FString Command;
        FString Arguments;
        if (CommandText.Split(TEXT(" "), &Command, &Arguments))
        {
            Command = Command.ToLower();

        	TArray<APlayerState*> Players = GetWorld()->GetGameState()->PlayerArray;

            // ---------------------- Message Privé ----------------------------
            if (Command == TEXT("/m") || Command == TEXT("/msg") || Command == TEXT("/whisper"))
            {
                FString RecipientName;
                FString PrivateMessage;
                if (Arguments.Split(TEXT(" "), &RecipientName, &PrivateMessage) && RecipientName != PlayerName)
                {
                    UE_LOG(LogTemp, Warning, TEXT("Destinataire : %s"), *RecipientName);
                    UE_LOG(LogTemp, Warning, TEXT("Message Privé : %s"), *PrivateMessage);

                	// New Message
                    NewMessage = PlayerName + TEXT(" <Warning> (privé)</>: ") + PrivateMessage;

                    FString Name;
                    for (APlayerState* Player : Players)
                    {
                        UAdvancedSessionsLibrary::GetPlayerName(Player->GetPlayerController(), Name);
                        if (Name == RecipientName)
                        {
	                        if (USChatComponent* ChatComponent = Player->GetPlayerController()->FindComponentByClass<USChatComponent>())
                            {
                                ChatComponent->Client_AddMessage(NewMessage, true);
                            	Client_AddMessage(NewMessage, true);
                                return true;
                            }
                        }
                    }

                    NewMessage = "<Error>" + PlayerName + " : Joueur non trouvé" + "</>";
                    Client_AddMessage(NewMessage, false);
                    return true;
                }
                else
                {
                    NewMessage = "<Error>Format de commande invalide. Utilisez /m <pseudo> <message></>";
                    Client_AddMessage(NewMessage, false);
                    return true;
                }
            }

            // ---------------------- Autres Commandes ---------------------------
            if (Command == TEXT("/team"))
            {
                // Implémenter la logique pour les messages d'équipe
                return true;
            }

            if (Command == TEXT("/help"))
            {
                // Implémenter la logique pour l'aide
                return true;
            }
        }

        NewMessage = "<Error>" + CommandText + " : Commande invalide" + "</>";
        Client_AddMessage(NewMessage, false);

        return true;
    }

    return false;
}


