#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "SMenuMultiplayerWidget.generated.h"


class USMenuJoinSessionWidget;
class USMenuhostSessionWidget;
class USButtonBaseWidget;

UCLASS(Abstract)
class MULTIPLAYERMENU_API USMenuMultiplayerWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual UWidget* NativeGetDesiredFocusTarget() const override;

protected:

	UFUNCTION()
	void OnHostButtonClicked();
	UFUNCTION()
	void OnJoinButtonClicked();
	UFUNCTION()
	void OnExitButtonClicked();
	

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USButtonBaseWidget* HostButton;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USButtonBaseWidget* JoinButton;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USButtonBaseWidget* ExitButton;

	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSoftClassPtr<USMenuhostSessionWidget> HostWidgetClass;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSoftClassPtr<USMenuJoinSessionWidget> JoinWidgetClass;
	
};
