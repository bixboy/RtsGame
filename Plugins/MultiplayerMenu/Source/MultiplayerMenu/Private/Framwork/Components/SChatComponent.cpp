#include "Framwork/Components/SChatComponent.h"
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
	TArray<APlayerState*> Players;
	Players = GetWorld()->GetGameState()->PlayerArray;
	
	for (APlayerState* Player : Players)
	{
		USChatComponent* ChatComponent = Player->GetPlayerController()->GetComponentByClass<USChatComponent>();
		ChatComponent->Client_AddMessage(NewMessage);
	}
}

void USChatComponent::Client_AddMessage_Implementation(const FString& NewMessage)
{
	ChatBox->AddChatMessage(NewMessage);
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


