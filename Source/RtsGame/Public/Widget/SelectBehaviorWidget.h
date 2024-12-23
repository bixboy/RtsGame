#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SelectBehaviorWidget.generated.h"

class UCustomButton;
class APlayerControllerRts;
class UBehaviorButtonWidget;

UCLASS()
class RTSGAME_API USelectBehaviorWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBehaviorButtonWidget* NeutralButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBehaviorButtonWidget* PassiveButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBehaviorButtonWidget* AggressiveButton;

protected:
	UFUNCTION()
	void OnBehaviorButtonClicked(UCustomButton* Button, int Index);

	UPROPERTY()
	APlayerControllerRts* PlayerController;
};
