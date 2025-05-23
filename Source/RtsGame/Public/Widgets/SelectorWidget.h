#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget/CustomButtonWidget.h"
#include "SelectorWidget.generated.h"

class UPlayerHudWidget;
class UToolTipWidget;
class USelectorWrapBox;
class UHorizontalBox;


USTRUCT(BlueprintType)
struct FGroupedActors
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> Actors;
};


UCLASS()
class RTSGAME_API USelectorWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UFUNCTION()
	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void ShowUnitEntries(TArray<AActor*> BuildsDataAssets);
	
	UFUNCTION()
	void ShowBuildEntries(TArray<AActor*> SelectedBuilds);

	UFUNCTION()
	void ClearSelectionWidget();

	UFUNCTION()
	bool GetIsOpen() { return bIsOpen; }

	UFUNCTION()
	void ShowToolTip(UDataAsset* Data);

	UFUNCTION()
	void HideToolTip();

	UFUNCTION()
	void SetParentWidget(UPlayerHudWidget* InParent) { ParentWidget = InParent; }

protected:
	
	// Components
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USelectorWrapBox* WrapBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBorder* SelectionBorder;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHorizontalBox* SelectionBox;
	
	UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidgetAnim))
	UWidgetAnimation* OpenSelection;

	UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidgetAnim))
	UWidgetAnimation* CloseSelection;

	// Variables
	UPROPERTY()
	bool bIsOpen = false;

	UPROPERTY()
	UPlayerHudWidget* ParentWidget;
};
