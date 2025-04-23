#include "Widgets/Entries/ResourceEntryWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"


void UResourceEntryWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	if (!TextName || !TextValue || !Image) return;
	
	if (CurrentResourceIcon)
	{
		Image->SetBrushFromTexture(CurrentResourceIcon);
	}
	else
	{
		Image->SetBrushFromTexture(nullptr);
	}

	TextName->SetText(ResourceName);
	TextValue->SetText(FText::FromString(FString::FromInt(CurrentResource)));
}

void UResourceEntryWidget::SetTextValue(int NewValue)
{
	CurrentResource = NewValue;
	TextValue->SetText(FText::FromString(FString::FromInt(NewValue)));
}
