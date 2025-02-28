#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "SPlayerLobbyWidget.generated.h"

class UListView;
class USPlayerLobbyEntry;

UCLASS(Abstract)
class MULTIPLAYERMENU_API USPlayerLobbyWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void AddPlayerToWidget(APlayerController* NewController);

protected:

	UPROPERTY()
	TArray<USPlayerLobbyEntry*> PlayerLobbyEntries;

	UPROPERTY(EditAnywhere, Category = "Settings|Widget")
	TSubclassOf<USPlayerLobbyEntry> PlayerLobbyEntryClass;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UListView* PlayerLobbyView;
	
};
