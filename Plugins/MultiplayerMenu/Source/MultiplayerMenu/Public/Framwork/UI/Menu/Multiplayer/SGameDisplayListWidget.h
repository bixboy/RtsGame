#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "SGameDisplayListWidget.generated.h"

class UVerticalBox;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameListCreatedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameSelectedDelegate, const FPrimaryAssetId&, SelectedGameData);

UCLASS(Abstract)
class MULTIPLAYERMENU_API USGameDisplayListWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	FOnGameListCreatedDelegate OnGameListCreated;
	FOnGameSelectedDelegate OnGameSelected;

	UFUNCTION()
	TArray<UWidget*> GetGameListWidgetList() const;

protected:
	UFUNCTION()
	void LoadGameList();
	
	UFUNCTION()
	void OnGameDataLoaded(TArray<FPrimaryAssetId> GameDataAssets);
	UFUNCTION()
	void OnGameListCreation();

	UFUNCTION()
	void OnGameDisplaySelection(const FPrimaryAssetId& SelectedGameData);

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* GameList;

	UPROPERTY()
	UCommonUserWidget* GameDisplayListWidget;
};
