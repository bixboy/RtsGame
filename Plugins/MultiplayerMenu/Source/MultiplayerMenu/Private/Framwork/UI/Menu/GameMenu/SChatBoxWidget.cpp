#include "Framwork/UI/Menu/GameMenu/SChatBoxWidget.h"
#include "AdvancedSessionsLibrary.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "Framwork/SLobbyPlayerController.h"
#include "Framwork/Components/SChatComponent.h"
#include "Framwork/UI/Menu/GameMenu/SChatEntry.h"
#include "GameFramework/GameStateBase.h"


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

	if (bIsInGame)
	{
		FInputModeUIOnly InputMode;
		GetWorld()->GetFirstPlayerController()->SetInputMode(InputMode);
	}
	else
	{
		FocusOnChat();
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

//------------------------------- New Message --------------------------------
#pragma region New Message
void USChatBoxWidget::AddChatMessage(FString NewMessage, bool bTTS)
{
	if (USChatEntry* NewChatEntry = CreateWidget<USChatEntry>(this, ChatEntry))
	{
		bTextToSpeech = bTTS;
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
    if (CommitMethod != ETextCommit::OnEnter)
    {
        return;
    }

    if (Text.IsEmpty()) return;

    FString PlayerName;
    UAdvancedSessionsLibrary::GetPlayerName(GetWorld()->GetFirstPlayerController(), PlayerName);

    FString NewMessage = PlayerName + ": " + Text.ToString();

    GetWorld()->GetTimerManager().ClearTimer(AutoCompleteTimerHandle);

    ChatComponent->Server_SendMessage(NewMessage);
    ExitChat();
}

#pragma endregion

//------------------------------- Auto Completion --------------------------------
#pragma region Auto Completion

void USChatBoxWidget::FindPlayer()
{
    const FString CurrentText = EditableText->GetText().ToString();
	
	UWorld* WorldContext = GetWorld();
	if (!WorldContext) return;

	AGameStateBase* GameState = WorldContext->GetGameState();
	if (!GameState) return;

    // Vérifie si l'utilisateur a modifié le texte (et non la mise à jour programmée)
    if (!bIsProgrammaticallyUpdatingText && !CurrentText.Equals(OriginalText))
    {
    	WorldContext->GetTimerManager().ClearTimer(AutoCompleteTimerHandle);
    	
        bIsAutoCompleting = false;
        AutoCompleteSuggestions.Empty();
        LastPartialText.Empty();
        AutoCompletePrefix.Empty();
        CurrentSuggestionIndex = 0;
        OriginalText = CurrentText;
        return;
    }

    // Si l'auto-complétion n'est pas active, la construire à partir du texte actuel
    if (!bIsAutoCompleting)
    {
        const FString PartialName = GetLastWord(CurrentText);
        if (PartialName.IsEmpty())
			return;

        LastPartialText = PartialName;
        AutoCompleteSuggestions.Empty();
        CurrentSuggestionIndex = 0;

        // Parcours des joueurs pour récupérer les suggestions
        for (APlayerState* Player : GameState->PlayerArray)
        {
            const FString PlayerName = Player->GetPlayerName();
            if (PlayerName.StartsWith(PartialName, ESearchCase::IgnoreCase))
            {
                AutoCompleteSuggestions.Add(PlayerName);
            }
        }

        UE_LOG(LogTemp, Warning, TEXT("Suggestions found: %d"), AutoCompleteSuggestions.Num());
        if (AutoCompleteSuggestions.Num() == 0)
			return;
        

        // Détermine le préfixe (tout le texte avant le dernier espace)
        int32 SpaceIndex;
        if (CurrentText.FindLastChar(' ', SpaceIndex))
			AutoCompletePrefix = CurrentText.Left(SpaceIndex + 1);
        else
			AutoCompletePrefix = TEXT("");

        bIsAutoCompleting = true;
    }

    // Cycle sur la liste des suggestions
    if (AutoCompleteSuggestions.Num() > 0)
    {
        CurrentSuggestionIndex = (CurrentSuggestionIndex + 1) % AutoCompleteSuggestions.Num();
        const FString CompletedName = AutoCompleteSuggestions[CurrentSuggestionIndex];
        UE_LOG(LogTemp, Warning, TEXT("CurrentSuggestionIndex: %d, CompletedName: %s"), CurrentSuggestionIndex, *CompletedName);

        const FString NewText = AutoCompletePrefix + CompletedName;
        
        bIsProgrammaticallyUpdatingText = true;
        EditableText->SetText(FText::FromString(NewText));
        OriginalText = NewText;
        bIsProgrammaticallyUpdatingText = false;

        // Redémarre le timer de verrouillage de 3 secondes
        WorldContext->GetTimerManager().ClearTimer(AutoCompleteTimerHandle);
        WorldContext->GetTimerManager().SetTimer(AutoCompleteTimerHandle, this, &USChatBoxWidget::OnAutoCompleteTimerExpired, 3.f, false);
    }
}

void USChatBoxWidget::OnAutoCompleteTimerExpired()
{
	UE_LOG(LogTemp, Warning, TEXT("AutoComplete timer expired"));
	
	bIsAutoCompleting = false;
	AutoCompleteSuggestions.Empty();
	LastPartialText.Empty();
	AutoCompletePrefix.Empty();
}

FString USChatBoxWidget::GetLastWord(const FString& InputText)
{
	FString TrimmedText = InputText.TrimEnd();
	int32 SpaceIndex;
	if (TrimmedText.FindLastChar(' ', SpaceIndex))
	{
		return TrimmedText.Mid(SpaceIndex + 1);
	}
	return TrimmedText;
}

#pragma endregion 
