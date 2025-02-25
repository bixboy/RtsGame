#pragma once

#include "CoreMinimal.h"
#include "CommonTabListWidgetBase.h"
#include "FindSessionsCallbackProxy.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "SGameSessionEntry.generated.h"


class UCommonSession_SearchResult;
struct FOnlineResultInformation;
class USGameSessionButton;

UCLASS(Abstract)
class MULTIPLAYERMENU_API USGameSessionEntry : public UCommonTabListWidgetBase, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

protected:
	UFUNCTION()
	void OnSessionSelected();
	
	UFUNCTION(BlueprintNativeEvent)
	void JoinSession(FBlueprintSessionResult Session);

	UFUNCTION()
	void OnGameDataLoaded();


	UPROPERTY()
	FBlueprintSessionResult SessionSearchResult;
	UPROPERTY()
	FPrimaryAssetId GameDataId;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USGameSessionButton* SessionButton;
	
	FDelegateHandle JoinSessionHandle;

};