﻿#pragma once

#include "CoreMinimal.h"
#include "PrimaryGameLayout.h"
#include "SPrimaryGameLayout.generated.h"

UCLASS()
class MULTIPLAYERMENU_API USPrimaryGameLayout : public UPrimaryGameLayout
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonActivatableWidgetStack* Game_Stack;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonActivatableWidgetStack* GameMenu_Stack;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonActivatableWidgetStack* Menu_Stack;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonActivatableWidgetStack* Modal_Stack;
};
