#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SChatWidget.generated.h"


class USChatBoxWidget;

UCLASS()
class MULTIPLAYERMENU_API USChatWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USChatBoxWidget* ChatBox;
};
