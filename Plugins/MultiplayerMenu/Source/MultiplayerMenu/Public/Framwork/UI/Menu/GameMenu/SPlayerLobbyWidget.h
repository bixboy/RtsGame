#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "SPlayerLobbyWidget.generated.h"

class USPlayerData;
struct FPlayerInfo;
class UListView;

UCLASS(Abstract)
class MULTIPLAYERMENU_API USPlayerLobbyWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void UpdatePlayerList(TArray<FPlayerInfo> PlayersList);

protected:
	UPROPERTY()
	TArray<USPlayerData*> PlayerLobbyEntries;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UListView* PlayerLobbyView;
};
