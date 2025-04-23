#include "Widgets/TopBarHudWidget.h"
#include "Widgets/PlayerResourceWidget.h"


void UTopBarHudWidget::UpdateResources(FResourcesCost NewResources)
{
	if (PlayerResource)
	{
		PlayerResource->UpdateResourceValue(NewResources);
	}
}
