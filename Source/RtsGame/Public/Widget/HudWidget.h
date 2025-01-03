﻿#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HudWidget.generated.h"

class UButton;
class UWidgetSwitcher;
class USelectBehaviorWidget;
class APlayerControllerRts;
class UFormationSelectorWidget;

UCLASS(Abstract)
class RTSGAME_API UHudWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UWidgetSwitcher* SelectorSwitcher;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UFormationSelectorWidget* FormationSelector;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USelectBehaviorWidget* BehaviorSelector;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Btn_SwitchBehavior;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Btn_SwitchFormation;

protected:
	UFUNCTION()
	void SetFormationSelectionWidget(const bool bEnabled) const;
	UFUNCTION()
	void SetBehaviorSelectionWidget(bool bEnabled) const;

	UFUNCTION()
	void OnSelectionUpdated();
	
	UPROPERTY()
	APlayerControllerRts* PlayerController;
};
