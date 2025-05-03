#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/DataRts.h"
#include "BuildInfoBox.generated.h"

class UBuildProgressWidget;
class AResourceDepot;
class USelectorWrapBox;
class UWrapBox;
class UEntryWidget;
class UUnitEntryWidget;
class UBuildResourceInfo;
class UTextBlock;
class UImage;


UCLASS()
class RTSGAME_API UBuildInfoBox : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void SetupBuildInfo(TArray<AActor*> Builds, USelectorWrapBox* Owner);

protected:
	UPROPERTY()
	USelectorWrapBox* OwnerWidget;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* BuildImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* BuildName;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* BuildDesc;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UWrapBox* WrapBox;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UBuildResourceInfo* BuildResource;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UBuildProgressWidget* BuildProgress;

	// -------
	
	UPROPERTY(EditAnywhere, Category = Settings)
	TSubclassOf<UUnitEntryWidget> UnitsProdEntryClass;

	UPROPERTY()
	TArray<UUnitEntryWidget*> UnitEntryList;

	UPROPERTY()
	TArray<AResourceDepot*> ResourceDepotList;

	// --------
	UFUNCTION()
	void CreateBuildProgressEntry(AActor* Build);
	
	UFUNCTION()
	void CreateResourceEntry(TArray<AActor*> SelectedBuilds);

	UFUNCTION()
	void UpdateResources(FResourcesCost ResourcesCost);
	
	UFUNCTION()
	void CreateUnitEntry(TArray<AActor*> ProductionBuilds);
	
	UFUNCTION()
	void OnUnitProduced(AActor* Actor);
	
};
