#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interaction3DWidget.generated.h"

class UEntryWidget;
class UWrapBox;


UCLASS()
class RTSGAME_API UInteraction3DWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION()
	TArray<UEntryWidget*> InitWidget(TArray<TSubclassOf<UEntryWidget>> EntriesClass);

protected:

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UWrapBox* WrapBox;
};
