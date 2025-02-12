#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "SMenuJoinSessionWidget.generated.h"


class UCommonBorder;
class USButtonBaseWidget;
class UCommonListView;
class UCommonSession_SearchSessionRequest;

UCLASS(Abstract)
class MULTIPLAYERMENU_API USMenuJoinSessionWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnActivated() override;
	virtual UWidget* NativeGetDesiredFocusTarget() const override;

protected:
	UFUNCTION()
	void StartSearch();
	
	UFUNCTION()
	void SetSpinnerDisplay(const bool bSpinnerState) const;
	
	UFUNCTION()
	void OnRefreshList();
	
	UFUNCTION()
	void OnSessionSearchComplete(bool bSucceeded, const FText& ErrorMessage, UCommonSession_SearchSessionRequest* SessionSearchRequest);
	
	UFUNCTION()
	void OnBackMenu();


	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonListView* ListView;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USButtonBaseWidget* RefreshButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USButtonBaseWidget* BackButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonBorder* NoSessionsDisplay;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonBorder* SpinnerDisplay;

	UPROPERTY()
	bool bSearchInProgress = false;
};
