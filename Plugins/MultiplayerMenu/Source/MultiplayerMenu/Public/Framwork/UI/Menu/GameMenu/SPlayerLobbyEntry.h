#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "SPlayerLobbyEntry.generated.h"

class ASLobbyPlayerController;
class UCommonTextBlock;


UCLASS(Abstract)
class MULTIPLAYERMENU_API USPlayerLobbyEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void SetPlayerReady(bool bReady);

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UCommonTextBlock* PlayerNameText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UCommonTextBlock* PlayerReadyText;

	UPROPERTY()
	ASLobbyPlayerController* Controller;

};
