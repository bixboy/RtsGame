#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "SMenuLobbyWidget.generated.h"

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
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USPlayerLobbyWidget* PlayerLobbyWidget;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USButtonSessionId* SessionIdButton;
};
