#include "Widgets/BuildsInfo//BuildResourceInfo.h"
#include "Widgets/Entries/ResourceEntryWidget.h"


void UBuildResourceInfo::UpdateResources(FResourcesCost NewResource, FResourcesCost MaxResource)
{
	if (WoodEntry)  WoodEntry->SetTextValue(NewResource.Woods, MaxResource.Woods);
	if (FoodEntry)  FoodEntry->SetTextValue(NewResource.Food, MaxResource.Food);
	if (MetalEntry) MetalEntry->SetTextValue(NewResource.Metal, MaxResource.Metal);
}
