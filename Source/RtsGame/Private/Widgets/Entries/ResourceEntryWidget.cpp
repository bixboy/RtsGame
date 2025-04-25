#include "Widgets/Entries/ResourceEntryWidget.h"

#include <comdef.h>

#include "Components/Image.h"
#include "Components/RichTextBlock.h"
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

	FSlateFontInfo NewFont = TextName->GetFont();
	NewFont.Size = TextSize;
	TextName->SetFont(NewFont);

	NewFont = TextValue->GetFont();
	NewFont.Size = TextSize;
	TextValue->SetFont(NewFont);

	TextName->SetText(ResourceName);
	//TextValue->SetText(FText::FromString(FString::FromInt(CurrentResource)));
}

void UResourceEntryWidget::SetTextValue(int NewValue)
{
	CurrentResource = NewValue;

	FString NewText = FString::FromInt(NewValue);
	TextValue->SetText(FText::FromString(NewText));
}

void UResourceEntryWidget::SetTextValue(int NewValue, int MaxValue)
{
	CurrentResource = NewValue;

	FString NewText = FString::FromInt(NewValue) + " / " + FString::FromInt(MaxValue);
	TextValue->SetText(FText::FromString(NewText));
}
