#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "SGameDisplayWidget.generated.h"

class USButtonGameWidget;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameSelectDisplayDelegate, const FPrimaryAssetId&, SelectedGameData);

UCLASS()
class MULTIPLAYERMENU_API USGameDisplayWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	
	UFUNCTION()
	void SetGameData(const FPrimaryAssetId& Data);
	UFUNCTION()
	FPrimaryAssetId GetGameDataAsset() const { return GameDataId; }

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USButtonGameWidget* GameButtonWidget;
	
	FOnGameSelectDisplayDelegate OnGameDisplaySelected;

protected:
	UFUNCTION()
	void OnGameSelection();
	
	UPROPERTY()
	FPrimaryAssetId GameDataId;
};
