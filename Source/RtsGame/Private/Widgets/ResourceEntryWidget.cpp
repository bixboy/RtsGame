#include "Widgets/ResourceEntryWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"


void UResourceEntryWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (Image && ResourceIcon)
	{
		Image->SetBrushFromTexture(ResourceIcon);
	}
	else if (Image)
	{
		Image->SetBrushFromTexture(nullptr);
	}

	TextName->SetText(ResourceName);
	TextValue->SetText(FText::FromString(FString::FromInt(0)));
}

void UResourceEntryWidget::SetTextValue(int NewValue)
{
	UE_LOG(LogTemp, Warning, TEXT("New Resources: Resource= %d"), NewValue);
	
	TextValue->SetText(FText::FromString(FString::FromInt(NewValue)));

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TextValue->GetText().ToString());
}
