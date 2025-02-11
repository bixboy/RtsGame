#pragma once

#include "CoreMinimal.h"
#include "Framwork/UI/Menu/SButtonBaseWidget.h"
#include "SButtonGameWidget.generated.h"

UCLASS(Abstract)
class MULTIPLAYERMENU_API USButtonGameWidget : public USButtonBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void SetGameData(const FPrimaryAssetId& GameDataAssetId);
	UFUNCTION()
	void UpdateButtonText(const FText& GameMapText, const FText& GameNameText, const FText& GameDescText);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonTextBlock* Name;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonTextBlock* Desc;
};
