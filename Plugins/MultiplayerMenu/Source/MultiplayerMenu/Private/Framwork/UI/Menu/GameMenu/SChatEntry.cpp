#include "Framwork/UI/Menu/GameMenu/SChatEntry.h"
#include "Components/RichTextBlock.h"


void USChatEntry::NativeConstruct()
{
	Super::NativeConstruct();

	if (Text)
		Text->SetAutoWrapText(true);
}

void USChatEntry::SetEntryText(FString NewText)
{
	if (Text)
		Text->SetText(FText::FromString(NewText));
}
