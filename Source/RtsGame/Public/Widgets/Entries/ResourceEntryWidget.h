#pragma once
#include "CoreMinimal.h"
#include "EntryWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/RtsResourcesComponent.h"
#include "Data/DataRts.h"
#include "ResourceEntryWidget.generated.h"

class UImage;
class UTextBlock;


UCLASS(Abstract)
class RTSGAME_API UResourceEntryWidget : public UEntryWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetTextValue(int NewValue);

protected:
	virtual void NativePreConstruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextValue;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* Image;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Settings)
	FText ResourceName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Icons")
	UTexture2D* CurrentResourceIcon;

	UPROPERTY()
	int CurrentResource;
};
