#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHudWidget.generated.h"

class UToolTipWidget;
class UTopBarHudWidget;
class USelectorWidget;


UCLASS()
class RTSGAME_API UPlayerHudWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION()
	virtual void NativeOnInitialized() override;
	
	UFUNCTION()
	void ShowToolTip(UDataAsset* Data);
	
	UFUNCTION()
	void HideToolTip();
	

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTopBarHudWidget* TopBarWidget;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	USelectorWidget* SelectorWidget;

protected:

	UFUNCTION()
	void ShowPendingToolTip();


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	TSubclassOf<UToolTipWidget> ToolTipClass;
	
	UPROPERTY()
	UToolTipWidget* ToolTipInfo;

	UPROPERTY()
	UDataAsset* PendingData;
	
	FTimerHandle ToolTipTimerHandle;
};
