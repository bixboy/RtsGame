#pragma once
#include "CoreMinimal.h"
#include "SelectorWidget.h"
#include "Components/WrapBox.h"
#include "SelectorWrapBox.generated.h"

class UBuildsEntry;
class UStructureDataAsset;
class UUnitsProductionDataAsset;
class USelectionEntryWidget;
class UUnitsInfoBox;
class UBuildInfoBox;


UCLASS()
class RTSGAME_API USelectorWrapBox : public UWrapBox
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void SetupWrapBox(TArray<AActor*> SelectedActors);

	UFUNCTION()
	void ClearWrapper();

	UFUNCTION()
	bool GetIsOpen();

protected:

	UFUNCTION()
	void SetupUnitEntries(TArray<AActor*> SelectedUnits);
	
	UFUNCTION()
	void SetupBuildEntries(TArray<AActor*> SelectedBuilds);

	UPROPERTY()
	USelectorWidget* SelectorWidget;

	
// ==== Builder Constructions ==== //

	UFUNCTION()
	void CreateBuilderEntry(TArray<UStructureDataAsset*> BuildList, TArray<AActor*> SelectedUnits);
	
	UFUNCTION()
	void CreateUnitInfoEntry(TArray<AActor*> Units);

	UFUNCTION()
	void CreateUnitsList(TMap<UUnitsProductionDataAsset*, FGroupedActors> UnitsMap);

	UPROPERTY(EditAnywhere, Category = Settings)
	TSubclassOf<UBuildInfoBox> BuildsInfoClass;

	UPROPERTY(EditAnywhere, Category = Settings)
	TSubclassOf<UBuildsEntry> BuildsEntryClass;

	UPROPERTY(EditAnywhere, Category = Settings)
	TSubclassOf<UUnitsInfoBox> UnitsInfoClass;

	UPROPERTY(EditAnywhere, Category = Settings)
	TSubclassOf<USelectionEntryWidget> UnitListEntryClass;

	UPROPERTY()
	UBuildInfoBox* BuildInfoBox;
	
	
// ==== Tool Tip ==== //
	
	UFUNCTION()
	void ShowToolTip(UDataAsset* Data);

	UFUNCTION()
	void HideToolTip();

public:
	UFUNCTION()
	void AddDelegateToolTip(UEntryWidget* Entry);
	
};
