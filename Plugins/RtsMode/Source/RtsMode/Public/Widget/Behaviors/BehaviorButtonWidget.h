#pragma once
#include "CoreMinimal.h"
#include "Widget/CustomButtonWidget.h"
#include "Data/AiData.h"
#include "BehaviorButtonWidget.generated.h"


UCLASS()
class RTSMODE_API UBehaviorButtonWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativePreConstruct() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	ECombatBehavior CombatBehavior;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCustomButtonWidget* Button;
};
