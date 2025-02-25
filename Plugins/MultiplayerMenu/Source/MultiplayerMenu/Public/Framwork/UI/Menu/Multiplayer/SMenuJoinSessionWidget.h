#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Framwork/Data/BlueprintSessionResultObject.h"
#include "SMenuJoinSessionWidget.generated.h"


struct FBlueprintSessionResult;
class UOverlay;
class UEditableTextBox;
class FUniqueNetIdString;
class UEditableText;
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
	
	virtual UWidget* NativeGetDesiredFocusTarget() const override;

	UFUNCTION()
	void SetActivated(bool bActivate);

protected:
	/*- Base Menu -*/
	
	UFUNCTION()
	void SetSpinnerDisplay(const bool bSpinnerState) const;
	
	UFUNCTION()
	void OnRefreshList();
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonListView* ListView;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USButtonBaseWidget* RefreshButton;

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USButtonBaseWidget* BackButton;
	
protected:
	/*- Join Session By ID -*/
	
	UFUNCTION()
	void OnJoinByID();

	UFUNCTION()
	void JoinSessionByID(const FString& NewSessionId);

	UFUNCTION(BlueprintNativeEvent)
	void OnJoinSessionIdIsFound();

	UFUNCTION()
	void OnSearchSessionById();

	UFUNCTION()
	void SetSessionIdDisplay(bool bNewDisplay) const;
	
	UPROPERTY(BlueprintReadOnly)
	FBlueprintSessionResult SessionById;

	
	/*- ID Component -*/
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USButtonBaseWidget* JoinByID;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USButtonBaseWidget* JoinSessionIdButton;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USButtonBaseWidget* BackSessionIdButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UOverlay* SessionIdOverlay;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* SessionIdEditableText;

	
	/*- Display No Session Found -*/

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonBorder* NoSessionsDisplay;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonBorder* SpinnerDisplay;

	UPROPERTY()
	bool bSearchInProgress = false;

	
	/*- Search Session -*/

	UFUNCTION()
	void StartSearch();
	
	UFUNCTION()
	void OnSessionSearchComplete(const TArray<FBlueprintSessionResult>& Results);

	UFUNCTION()
	void OnSessionSearchFailed(const TArray<FBlueprintSessionResult>& Results);
	
	UPROPERTY()
	TArray<FBlueprintSessionResult> SessionsList;
	
};
