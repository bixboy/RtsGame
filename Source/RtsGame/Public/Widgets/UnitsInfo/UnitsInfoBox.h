#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UnitsInfoBox.generated.h"

class UBuildsEntry;
class UStructureDataAsset;
class USelectorWrapBox;
class UWrapBox;
class UUnitsProductionDataAsset;
class UTextBlock;
class UImage;


UCLASS()
class RTSGAME_API UUnitsInfoBox : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void SetupUnitInfos(UUnitsProductionDataAsset* UnitData);

	UFUNCTION(BlueprintCallable)
	void ShowBuildList(TArray<UStructureDataAsset*> BuildList, USelectorWrapBox* Owner);
	
protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* UnitImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* UnitName;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* UnitDesc;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UWrapBox* WrapBox;


	UPROPERTY(EditAnywhere, Category = Settings)
	TSubclassOf<UBuildsEntry> BuildsEntryClass;
};
