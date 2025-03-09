#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "SPlayerLobbyEntry.generated.h"

class USPlayerData;
class UTextBlock;
class ASLobbyPlayerController;


UCLASS(Abstract)
class MULTIPLAYERMENU_API USPlayerLobbyEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	
	UFUNCTION()
	void UpdateEntry();

	UFUNCTION()
	void SetPlayerReady();

protected:
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* PlayerNameText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* PlayerReadyText;

	UPROPERTY()
	ASLobbyPlayerController* Controller;

	UPROPERTY()
	USPlayerData* PlayerData;
};
