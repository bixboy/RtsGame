#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CustomButtonWidget.generated.h"

class UTextBlock;
class UImage;
class UButton;
class UCustomButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FButtonClicked, UCustomButton*, Button, int, Index);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FButtonHovered, UCustomButton*, Button, int, Index);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FButtonUnHovered, UCustomButton*, Button, int, Index);

UCLASS()
class RTSMODE_API UCustomButton : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Button;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* ButtonImage;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* ButtonText;

	UPROPERTY(BlueprintReadOnly, BlueprintCallable)
	FButtonClicked OnButtonClicked;
	UPROPERTY(BlueprintReadOnly, BlueprintCallable)
	FButtonHovered OnButtonHovered;
	UPROPERTY(BlueprintReadOnly, BlueprintCallable)
	FButtonUnHovered OnButtonUnHovered;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Settings)
	int ButtonIndex;

private:
	UFUNCTION()
	void OnCustomUIButtonClickedEvent();
	UFUNCTION()
	void OnCustomUIButtonHoveredEvent();
	UFUNCTION()
	void OnCustomUIButtonUnHoveredEvent();
};
