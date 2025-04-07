#include "Widgets/PlayerResourceWidget.h"
#include "Components/RtsComponent.h"
#include "Components/RtsResourcesComponent.h"
#include "Player/RtsPlayerController.h"
#include "Structures/ResourceDepot.h"
#include "Widgets/ResourceEntryWidget.h"


UPlayerResourceWidget::UPlayerResourceWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	UE_LOG(LogTemp, Warning, TEXT("UPlayerResourceWidget instance created: %p"), this);
}

void UPlayerResourceWidget::UpdateResourceValue(const FResourcesCost& NewResources)
{
	if (Wood)
	{
		Wood->SetTextValue(NewResources.Woods);
	}
	if (Food)
	{
		Food->SetTextValue(NewResources.Food);
	}
	if (Metal)
	{
		Metal->SetTextValue(NewResources.Metal);
	}
}
