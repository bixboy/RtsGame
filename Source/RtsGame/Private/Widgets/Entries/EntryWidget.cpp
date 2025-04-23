#include "Widgets/Entries/EntryWidget.h"
#include "Player/RtsPlayerController.h"
#include "Widget/CustomButtonWidget.h"


void UEntryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	verify((PlayerController = Cast<ARtsPlayerController>(GetWorld()->GetFirstPlayerController())));

	if (CustomButton)
	{
		CustomButton->OnButtonHovered.AddDynamic(this, &UEntryWidget::OnEntryHover);
		CustomButton->OnButtonUnHovered.AddDynamic(this, &UEntryWidget::OnEntryUnHover);
	}
}

void UEntryWidget::RemoveFromParent()
{
	OnEntryHovered.Clear();
	OnEntryUnHovered.Clear();
	
	Super::RemoveFromParent();
}

void UEntryWidget::InitializeEntry(UDataAsset* NewData)
{
	DataAsset = NewData;
}

void UEntryWidget::OnEntryHover(UCustomButtonWidget* Button, int Index)
{
	OnEntryHovered.Broadcast(DataAsset);
}

void UEntryWidget::OnEntryUnHover(UCustomButtonWidget* Button, int Index)
{
	OnEntryUnHovered.Broadcast();
}
