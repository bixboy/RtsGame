#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ResourceEntryWidget.generated.h"


class UImage;
class UTextBlock;

UCLASS(Abstract)
class RTSGAME_API UResourceEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextValue;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* Image;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Settings)
	FText ResourceName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Settings)
	UTexture2D* ResourceIcon;

	UFUNCTION(BlueprintCallable)
	void SetTextValue(int NewValue);

protected:
	virtual void NativePreConstruct() override;
};
