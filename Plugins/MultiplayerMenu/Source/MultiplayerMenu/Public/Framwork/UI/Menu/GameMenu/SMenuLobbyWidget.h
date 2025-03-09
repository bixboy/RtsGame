#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "SMenuLobbyWidget.generated.h"

class USButtonBaseWidget;
class UButton;
class USPlayerLobbyWidget;
class USButtonSessionId;

UCLASS()
class MULTIPLAYERMENU_API USMenuLobbyWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	UFUNCTION(BlueprintCallable)
	USPlayerLobbyWidget* GetPlayerLobbyWidget();

protected:

	UFUNCTION()
	void OnMenuPressed();

	UFUNCTION()
	void OnReadyPressed();

	UFUNCTION(BlueprintImplementableEvent, Category = Sessions)
	void DestroySession(APlayerController* Controller);
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FString GetLevelPath(FSoftObjectPath Level);
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USPlayerLobbyWidget* PlayerLobbyWidget;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USButtonSessionId* SessionIdButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USButtonBaseWidget* MenuButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USButtonBaseWidget* ReadyToggleButton;
};
