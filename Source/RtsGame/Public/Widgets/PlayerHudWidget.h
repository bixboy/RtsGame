#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHudWidget.generated.h"

class UTopBarHudWidget;
class USelectorWidget;


UCLASS()
class RTSGAME_API UPlayerHudWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTopBarHudWidget* TopBarWidget;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	USelectorWidget* SelectorWidget;

protected:
	
};
