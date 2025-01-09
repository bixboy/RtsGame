#include "Widget/FormationButtonWidget.h"

#include "Components/TextBlock.h"

void UFormationButtonWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	ButtonIndex = static_cast<int>(Formation);

	if (ButtonText)
	{
		ButtonText->SetText(UEnum::GetDisplayValueAsText(Formation));
	}
}
