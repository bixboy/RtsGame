#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/DataRts.h"
#include "BuildsEntry.generated.h"

class ARtsPlayerController;
class UStructureDataAsset;
class UCustomButtonWidget;
class APlayerControllerRts;


UCLASS()
class RTSGAME_API UBuildsEntry : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void InitEntry(UStructureDataAsset* DataAsset);
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCustomButtonWidget* BuildButton;

protected:

	UFUNCTION()
	void OnBuildSelected(UCustomButtonWidget* Button, int Index);

	UPROPERTY()
	FStructure BuildData;

	UPROPERTY()
	ARtsPlayerController* PlayerController;
};
