#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HudWidget.generated.h"

class APlayerControllerRts;
class UFormationSelectorWidget;

UCLASS(Abstract)
class RTSGAME_API UHudWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UFormationSelectorWidget* FormationSelector;

protected:
	UFUNCTION()
	void SetFormationSelectionWidget(const bool bEnabled) const;

	UFUNCTION()
	void OnSelectionUpdated();
	
	UPROPERTY()
	APlayerControllerRts* PlayerController;
};
