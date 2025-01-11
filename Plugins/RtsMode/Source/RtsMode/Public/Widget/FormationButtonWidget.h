#pragma once

#include "CoreMinimal.h"
#include "CustomButtonWidget.h"
#include "Data/AiData.h"
#include "FormationButtonWidget.generated.h"

UCLASS()
class RTSMODE_API UFormationButtonWidget : public UCustomButton
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TEnumAsByte<EFormation> Formation;
};
