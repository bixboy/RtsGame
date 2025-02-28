#pragma once

#include "CoreMinimal.h"
#include "Framwork/UI/Menu/SButtonBaseWidget.h"
#include "SButtonSessionId.generated.h"

UCLASS(Abstract)
class MULTIPLAYERMENU_API USButtonSessionId : public USButtonBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;


protected:
	UFUNCTION()
	void OnIdClicked();
	
};
