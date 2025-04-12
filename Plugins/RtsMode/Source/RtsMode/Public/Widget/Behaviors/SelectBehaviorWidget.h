#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SelectBehaviorWidget.generated.h"

class UCustomButtonWidget;
class APlayerControllerRts;
class UBehaviorButtonWidget;


UCLASS()
class RTSMODE_API USelectBehaviorWidget : public UUserWidget
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
	void OnBehaviorButtonClicked(UCustomButtonWidget* Button, int Index);

	UFUNCTION()
	void UpdateSelectedButton(UCustomButtonWidget* Button, bool IsSelected);

	UFUNCTION()
	void OnNewUnitSelected(TArray<AActor*> NewSelection);

	UPROPERTY()
	APlayerControllerRts* PlayerController;
};
