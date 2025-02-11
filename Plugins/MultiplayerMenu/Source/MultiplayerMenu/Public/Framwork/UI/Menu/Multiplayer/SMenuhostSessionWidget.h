#pragma once

#include "CommonSessionSubsystem.h"
#include "CommonActivatableWidget.h"
#include "CommonUserSubsystem.h"
#include "SMenuhostSessionWidget.generated.h"

class USButtonBaseWidget;
class UCommonTextBlock;
class USGameDisplayListWidget;

UENUM()
enum ESetting1
{
	Set1,
	Set2
};

UENUM()
enum ESetting2
{
	Set3,
	Set4
};

UCLASS(Abstract)
class MULTIPLAYERMENU_API USMenuhostSessionWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

protected:
	UFUNCTION()
	void InitTextDisplays() const;
	
	UFUNCTION()
	void AttenptOnlineLogin();
	
	UFUNCTION()
	void OnUserOnlineLogin(const UCommonUserInfo* UserInfo, bool bSuccess, FText Error,
							ECommonUserPrivilege RequestedPrivilege, ECommonUserOnlineContext OnlineContext);

	
	UFUNCTION()
	void HostSession();
	
	UFUNCTION()
	UCommonSession_HostSessionRequest* CreateHostingRequest() const;
	
	UFUNCTION()
	void OnSessionCreated(const FOnlineResultInformation& OnlineResultInformation);

	UPROPERTY()
	FPrimaryAssetId GameDataId;

	// Game List
	UFUNCTION()
	void OnGameListCreated();
	UFUNCTION()
	void OnGameSelected(const FPrimaryAssetId& SelectedGameData);
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USGameDisplayListWidget* GameList;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonTextBlock* SelectedGameTitleText;

	// Launch
	UFUNCTION()
	void OnlaunchGame();
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USButtonBaseWidget* LaunchButton;

	// Back
	UFUNCTION()
	void OnBackGame();
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USButtonBaseWidget* BackButton;

	/** General Settings **/
	UPROPERTY()
	TEnumAsByte<ESetting1> Setting1;
	UPROPERTY()
	TEnumAsByte<ESetting2> Setting2;


	UFUNCTION()
	void OnSetting1Changed();
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonTextBlock* GameSetting1Text;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USButtonBaseWidget* ChangeSetting1Button;

	UFUNCTION()
	void OnSetting2Changed();
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonTextBlock* GameSetting2Text;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USButtonBaseWidget* ChangeSetting2Button;

	/** NetMode Settings **/
	UFUNCTION()
	void OnNetWorkModeButtonClicked();

	UPROPERTY()
	ECommonSessionOnlineMode NetMode;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonTextBlock* NetWorkModeText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USButtonBaseWidget* ChangeNetModeButton;
	
};
