#pragma once
#include "CoreMinimal.h"
#include "EntryWidget.h"
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

	void SetTextValue(int NewValue, int MaxValue);

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	UTexture2D* CurrentResourceIcon;

	UPROPERTY()
	int CurrentResource;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	float TextSize = 24.f;
};
