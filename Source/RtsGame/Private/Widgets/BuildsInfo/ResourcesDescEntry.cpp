#include "Widgets/BuildsInfo//BuildResourceInfo.h"
#include "Widgets/Entries/ResourceEntryWidget.h"


void UBuildResourceInfo::UpdateResources(FResourcesCost NewResource)
{
	if (WoodEntry)  WoodEntry ->SetTextValue(NewResource.Woods);
	if (FoodEntry)  FoodEntry ->SetTextValue(NewResource.Food);
	if (MetalEntry) MetalEntry->SetTextValue(NewResource.Metal);
}
