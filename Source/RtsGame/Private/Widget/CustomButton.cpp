#include "Components/Button.h"
#include "Widget/CustomButtonWidget.h"

void UCustomButton::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Button->OnClicked.AddDynamic(this, &UCustomButton::OnCustomUIButtonClickedEvent);
	Button->OnHovered.AddDynamic(this, &UCustomButton::OnCustomUIButtonHoveredEvent);
	Button->OnUnhovered.AddDynamic(this, &UCustomButton::OnCustomUIButtonUnHoveredEvent);
}

void UCustomButton::OnCustomUIButtonClickedEvent()
{
	OnButtonClicked.Broadcast(this, ButtonIndex);
}

void UCustomButton::OnCustomUIButtonHoveredEvent()
{
	OnButtonHovered.Broadcast(this, ButtonIndex);
}

void UCustomButton::OnCustomUIButtonUnHoveredEvent()
{
	OnButtonUnHovered.Broadcast(this, ButtonIndex);
}
