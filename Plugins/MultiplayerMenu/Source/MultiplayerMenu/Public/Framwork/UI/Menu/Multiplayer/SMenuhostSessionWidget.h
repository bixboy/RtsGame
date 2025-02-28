#pragma once

#include "CommonSessionSubsystem.h"
#include "CommonActivatableWidget.h"
#include "CommonUserSubsystem.h"
#include "SMenuhostSessionWidget.generated.h"

class UEditableText;
class USlider;
class USButtonBaseWidget;
class UCommonTextBlock;
class USGameDisplayListWidget;

USTRUCT()
struct FGameSettings
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FString GameName = "None";

	UPROPERTY()
	FString MapName = "In Selection";
	
	UPROPERTY()
	int32 MaxPlayers = 2;
	
	UPROPERTY()
	int PlayerLife = 2;

	UPROPERTY()
	float RoundDuration;
	
};

UENUM()
enum ESettingMaxPlayers
{
	Max1,
	Max2,
	Max3
};

UENUM()
enum ESettingPlayerHealth
{
	PlayerHealth1,
	PlayerHealth2,
	PlayerHealth3,
	PlayerHealth4,
	PlayerHealth5,
	PlayerHealth6,
	PlayerHealth7,
};

UCLASS(Abstract)
class MULTIPLAYERMENU_API USMenuhostSessionWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void SetActivated(bool bActivate);

	UPROPERTY(EditAnywhere)
	FPrimaryAssetId LobbyLevel;

	UFUNCTION(BlueprintImplementableEvent, Category = Sessions)
	void OnCreateSessionSuccess();

protected:
	UFUNCTION()
	void InitTextDisplays();

	UFUNCTION()
	void HostSession();

	UFUNCTION()
	void OnCreateSessionFailure();

	UFUNCTION()
	void OnCreteSessionSuccessGenerateId();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FString GenerateTruncatedGuid();

	UPROPERTY()
	FPrimaryAssetId GameDataId;

	
	/*- Game Name -*/
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableText* SelectedGameTitleText;

	UFUNCTION()
	void OnGameNameChange(const FText& Text, ETextCommit::Type CommitMethod);

	
	/*- Launch Game -*/
	UFUNCTION()
	void OnLaunchGame();
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USButtonBaseWidget* LaunchButton;

public:
	// Back
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USButtonBaseWidget* BackButton;

	
protected:
	/*- General Settings -*/
	UPROPERTY()
	TEnumAsByte<ESettingMaxPlayers> Setting1;
	UPROPERTY()
	TEnumAsByte<ESettingPlayerHealth> Setting2;
	

	UPROPERTY()
	FGameSettings GameSettings;

	//Setting 1
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USButtonBaseWidget* ChangeSetting1Button;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonTextBlock* GameSetting1Text;

	UFUNCTION()
	void OnSetting1Changed();
	
	//Setting 2
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USButtonBaseWidget* ChangeSetting2Button;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonTextBlock* GameSetting2Text;

	UFUNCTION()
	void OnSetting2Changed();

	//Setting 3
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* TimeSlider;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonTextBlock* GameSetting3Text;

	UFUNCTION()
	void OnSliderChange(float Value);

	/*- NetMode Settings -*/
	UFUNCTION()
	void OnNetWorkModeButtonClicked();

	UPROPERTY()
	ECommonSessionOnlineMode NetMode;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonTextBlock* NetWorkModeText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USButtonBaseWidget* ChangeNetModeButton;

	/*- Utility -*/
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FString GetLevelPath(FSoftObjectPath Level);
	
};
