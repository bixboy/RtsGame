#include "Widgets/PlayerResourceWidget.h"
#include "Components/RtsComponent.h"
#include "Widgets/Entries/ResourceEntryWidget.h"


void UPlayerResourceWidget::UpdateResourceValue(const FResourcesCost NewResources)
{
	if (Wood)
	{
		Wood->SetTextValue(NewResources.Woods);
		UE_LOG(LogTemp, Warning, TEXT("New Wood Widget: %d"), NewResources.Woods);
	}
	if (Food)
	{
		Food->SetTextValue(NewResources.Food);
		UE_LOG(LogTemp, Warning, TEXT("New Food Widget: %d"), NewResources.Food);
	}
	if (Metal)
	{
		Metal->SetTextValue(NewResources.Metal);
		UE_LOG(LogTemp, Warning, TEXT("New Metal Widget: %d"), NewResources.Metal);
	}
}
