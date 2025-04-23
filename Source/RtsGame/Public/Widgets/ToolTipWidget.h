#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ToolTipWidget.generated.h"


class UStructureDataAsset;
class UUnitsProductionDataAsset;
class UTextBlock;
class UImage;

UCLASS()
class RTSGAME_API UToolTipWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION()
	void ShowToolTip(UDataAsset* Data);

	UFUNCTION()
	void HideToolTip();

protected:

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* ToolTipImage;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TitleText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* SubTitleText;

	UPROPERTY()
	bool bFollowMouse = false;
};
