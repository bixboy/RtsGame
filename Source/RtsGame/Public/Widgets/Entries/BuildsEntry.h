#pragma once
#include "CoreMinimal.h"
#include "EntryWidget.h"
#include "BuildsEntry.generated.h"

class ARtsPlayerController;
class UStructureDataAsset;
class UCustomButtonWidget;
class APlayerControllerRts;


UCLASS()
class RTSGAME_API UBuildsEntry : public UEntryWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	virtual void InitializeEntry(UDataAsset* NewDataAsset) override;

protected:
	UFUNCTION()
	void OnBuildSelected(UCustomButtonWidget* Button, int Index);
	
	UPROPERTY()
	UStructureDataAsset* BuildData;
	
};
