#include "Widget/Formations/FormationButtonWidget.h"
#include "Components/TextBlock.h"

void UFormationButtonWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	ButtonIndex = static_cast<int8>(Formation.GetValue());

	if (ButtonText)
	{
		ButtonText->SetText(UEnum::GetDisplayValueAsText(Formation));
	}
}
