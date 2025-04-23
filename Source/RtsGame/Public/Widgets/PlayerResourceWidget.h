#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/DataRts.h"
#include "PlayerResourceWidget.generated.h"


class AResourceDepot;
class ARtsPlayerController;
class UResourceEntryWidget;

UCLASS(Abstract)
class RTSGAME_API UPlayerResourceWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPlayerResourceWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UResourceEntryWidget* Wood;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UResourceEntryWidget* Food;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UResourceEntryWidget* Metal;

	UFUNCTION()
	void UpdateResourceValue(const FResourcesCost NewResources);

protected:
	UPROPERTY()
	ARtsPlayerController* PlayerController;
};
