#pragma once

#include "CoreMinimal.h"
#include "CustomButtonWidget.h"
#include "Data/AiData.h"
#include "BehaviorButtonWidget.generated.h"

UCLASS()
class RTSMODE_API UBehaviorButtonWidget : public UCustomButton
{
	GENERATED_BODY()

	virtual void NativePreConstruct() override;
	
public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TEnumAsByte<ECombatBehavior> CombatBehavior;
};
