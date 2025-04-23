#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/DataRts.h"
#include "TopBarHudWidget.generated.h"

class UPlayerResourceWidget;


UCLASS()
class RTSGAME_API UTopBarHudWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateResources(FResourcesCost NewResources);

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UPlayerResourceWidget* PlayerResource;
};
