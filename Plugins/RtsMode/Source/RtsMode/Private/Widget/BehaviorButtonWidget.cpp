#include "Widget/BehaviorButtonWidget.h"

#include "Components/TextBlock.h"

void UBehaviorButtonWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	ButtonIndex = static_cast<int8>(CombatBehavior.GetValue());

	if (ButtonText)
	{
		ButtonText->SetText(UEnum::GetDisplayValueAsText(CombatBehavior));
	}
}
