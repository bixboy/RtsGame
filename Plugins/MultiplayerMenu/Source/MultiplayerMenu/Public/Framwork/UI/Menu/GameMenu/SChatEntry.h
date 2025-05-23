﻿#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SChatEntry.generated.h"


class URichTextBlock;

UCLASS()
class MULTIPLAYERMENU_API USChatEntry : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void SetEntryText(FString NewText);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	URichTextBlock* Text;
};
