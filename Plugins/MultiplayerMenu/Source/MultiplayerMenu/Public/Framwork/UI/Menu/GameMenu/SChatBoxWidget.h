﻿#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SChatBoxWidget.generated.h"

class USChatEntry;
class USChatComponent;
class UScrollBox;
class UEditableText;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNewMessageReceived, const FString&, Message);

UCLASS()
class MULTIPLAYERMENU_API USChatBoxWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:	
	virtual void NativeConstruct() override;

	UFUNCTION()
	void AddChatMessage(FString NewMessage, bool bTTS);

	UFUNCTION()
	void Scroll(bool bUp);

	UFUNCTION()
	void FocusOnChat();

	UFUNCTION()
	void FindPlayer();

	UFUNCTION()
	FString GetLastWord(const FString& InputText);

	UPROPERTY(BlueprintAssignable)
	FOnNewMessageReceived OnNewMessageReceived;

protected:
	UFUNCTION()
	void ExitChat();
	
	UFUNCTION()
	void OnTextSend(const FText& Text, ETextCommit::Type CommitMethod);

	UPROPERTY()
	bool bChatIsFocused;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableText* EditableText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UScrollBox* ScrollBox;

	UPROPERTY(EditAnywhere, Category = "Settings|Chat")
	bool bIsInGame = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Chat")
	bool bTextToSpeech = false;

	UPROPERTY(EditAnywhere, Category = "Settings|Chat")
	float ScrollMultiplier = 60.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Chat")
	TSubclassOf<USChatEntry> ChatEntry;

	UPROPERTY()
	USChatComponent* ChatComponent;

// --------------------- Auto Complete ------------------------
#pragma region Auto Complete

	UFUNCTION()
	void OnAutoCompleteTimerExpired();

	UPROPERTY()
	TArray<FString> AutoCompleteSuggestions;
	
	UPROPERTY()
	int32 CurrentSuggestionIndex = 0;
	
	UPROPERTY()
	FString LastPartialText;

	UPROPERTY()
	FString OriginalText;

	UPROPERTY()
	FString AutoCompletePrefix;
	
	UPROPERTY()
	bool bIsAutoCompleting = false;

	bool bIsProgrammaticallyUpdatingText = false;
	
	UPROPERTY()
	FTimerHandle AutoCompleteTimerHandle;

#pragma endregion	
};
