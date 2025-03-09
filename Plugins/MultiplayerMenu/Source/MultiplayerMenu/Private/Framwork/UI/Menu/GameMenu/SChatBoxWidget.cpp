#include "Framwork/UI/Menu/GameMenu/SChatBoxWidget.h"
#include "AdvancedSessionsLibrary.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "Framwork/SLobbyPlayerController.h"
#include "Framwork/Components/SChatComponent.h"
#include "Framwork/UI/Menu/GameMenu/SChatEntry.h"


void USChatBoxWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ASLobbyPlayerController* Controller = Cast<ASLobbyPlayerController>(GetWorld()->GetFirstPlayerController()))
		ChatComponent = Controller->GetChatComponent();

	if (EditableText)
	{
		EditableText->OnTextCommitted.AddDynamic(this, &USChatBoxWidget::OnTextSend);

		if (bIsInGame)
			EditableText->SetIsEnabled(false);
		else
			EditableText->SetIsEnabled(true);
	}
}

void USChatBoxWidget::FocusOnChat()
{
	EditableText->SetUserFocus(GetWorld()->GetFirstPlayerController());
	EditableText->SetIsEnabled(true);
	bChatIsFocused = true;
}

void USChatBoxWidget::ExitChat()
{
	bChatIsFocused = false;
	EditableText->SetText(FText());
	EditableText->SetIsEnabled(false);

	if (bIsInGame)
	{
		FInputModeUIOnly InputMode;
		GetWorld()->GetFirstPlayerController()->SetInputMode(InputMode);
	}
}

void USChatBoxWidget::Scroll(bool bUp)
{
	float Multiplier;
	if (bUp)
		Multiplier = -1.f;
	else
		Multiplier = 1.f;

	Multiplier = Multiplier * ScrollMultiplier;
	Multiplier = Multiplier + ScrollBox->GetScrollOffset();

	Multiplier = FMath::Clamp(Multiplier, 0.f, ScrollBox->GetScrollOffsetOfEnd());
	
	ScrollBox->SetScrollOffset(Multiplier);
}

void USChatBoxWidget::AddChatMessage(FString NewMessage)
{
	if (USChatEntry* NewChatEntry = CreateWidget<USChatEntry>(this, ChatEntry))
	{
		NewChatEntry->SetEntryText(NewMessage);
		
		ScrollBox->AddChild(NewChatEntry);
		ScrollBox->ScrollToEnd();

		if (bTextToSpeech)
		{
			FString LeftPart;
			FString RightPart;
			NewMessage.Split(TEXT(":"), &LeftPart, &RightPart);
		
			OnNewMessageReceived.Broadcast(RightPart);	
		}
	}
}

void USChatBoxWidget::OnTextSend(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod != ETextCommit::Type::OnEnter && Text.IsEmpty()) return;

	FString PlayerName;
	UAdvancedSessionsLibrary::GetPlayerName(GetWorld()->GetFirstPlayerController(), PlayerName);
	
	FString NewMessage = PlayerName + ": " + Text.ToString();

	ChatComponent->Server_SendMessage(NewMessage);
	ExitChat();
}
