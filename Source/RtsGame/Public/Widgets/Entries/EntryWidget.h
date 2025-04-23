#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EntryWidget.generated.h"

class ARtsPlayerController;
class UCustomButtonWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEntryUnHover);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEntryHover, UDataAsset*, DataAsset);


UCLASS()
class RTSGAME_API UEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UFUNCTION()
	virtual void InitializeEntry(UDataAsset* NewData);
	
	UPROPERTY(BlueprintReadOnly, BlueprintCallable)
	FEntryUnHover OnEntryUnHovered;

	UPROPERTY(BlueprintReadOnly, BlueprintCallable)
	FEntryHover OnEntryHovered;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCustomButtonWidget* CustomButton;
	
protected:
	
	UFUNCTION()
	virtual void NativeOnInitialized() override;

	virtual void RemoveFromParent() override;
	
	UFUNCTION()
	virtual void OnEntryHover(UCustomButtonWidget* Button, int Index);

	UFUNCTION()
	void OnEntryUnHover(UCustomButtonWidget* Button, int Index);

	UPROPERTY()
	ARtsPlayerController* PlayerController;

	UPROPERTY()
	UDataAsset* DataAsset;
};
