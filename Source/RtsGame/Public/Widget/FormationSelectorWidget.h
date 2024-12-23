#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FormationSelectorWidget.generated.h"

class APlayerControllerRts;
class UCustomButton;
class USlider;
class UFormationButtonWidget;

UCLASS()
class RTSGAME_API UFormationSelectorWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UFormationButtonWidget* LineButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UFormationButtonWidget* ColumnButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UFormationButtonWidget* WedgeButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UFormationButtonWidget* BlobButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* SpacingSlider;

protected:
	UFUNCTION()
	void OnFormationButtonClicked(UCustomButton* Button, int Index);

	UFUNCTION()
	void OnSpacingSliderValueChanged(const float Value);

	UPROPERTY()
	APlayerControllerRts* PlayerController;
};
