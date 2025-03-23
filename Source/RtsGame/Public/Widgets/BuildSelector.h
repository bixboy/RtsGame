#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BuildSelector.generated.h"


class UBuildsEntry;
class UStructureDataAsset;
class UCustomButtonWidget;
class UBorder;
class UWrapBox;
class UButton;

UCLASS()
class RTSGAME_API UBuildSelector : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;
	
	UFUNCTION()
	void SetupBuildsList();
	
protected:

	UFUNCTION()
	void OnShowButtonPressed();

	UFUNCTION()
	void OnBuildSelected(UCustomButtonWidget* Button, int Index);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	TSubclassOf<UBuildsEntry> BuildsEntryClass;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Btn_ShowBuildSelector;
	
	UPROPERTY(EditAnywhere, Category = "Settings")
	TArray<UStructureDataAsset*> BuildsDataAssets;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWrapBox* WrapBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBorder* ListBorder;

	UPROPERTY()
	TArray<UBuildsEntry*> EntryList;
};
