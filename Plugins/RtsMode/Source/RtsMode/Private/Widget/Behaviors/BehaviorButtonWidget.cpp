#include "Widget/Behaviors/BehaviorButtonWidget.h"
#include "Components/TextBlock.h"

void UBehaviorButtonWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	const UEnum* EnumPtr = StaticEnum<ECombatBehavior>();
	ButtonIndex = static_cast<int32>(CombatBehavior);

	if (ButtonText)
	{
		ButtonText->SetText(EnumPtr->GetDisplayNameTextByValue(ButtonIndex));
	}
}
