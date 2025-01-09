#include "Widget/BehaviorButtonWidget.h"

#include "Components/TextBlock.h"

void UBehaviorButtonWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	ButtonIndex = TEnumAsByte<ECombatBehavior>(CombatBehavior).GetIntValue();

	if (ButtonText)
	{
		ButtonText->SetText(UEnum::GetDisplayValueAsText(CombatBehavior));
	}
}
