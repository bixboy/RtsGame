#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/DataRts.h"
#include "BuildResourceInfo.generated.h"

class UResourceEntryWidget;


UCLASS()
class RTSGAME_API UBuildResourceInfo : public UUserWidget 
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void UpdateResources(FResourcesCost NewResource);

protected:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UResourceEntryWidget* WoodEntry;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UResourceEntryWidget* FoodEntry;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UResourceEntryWidget* MetalEntry;
};
