#include "Widgets/3DWidget/Interaction3DWidget.h"
#include "Components/WrapBox.h"
#include "Widgets/UnitsInfo/UnitsInfoBox.h"
#include "Widgets/Entries/EntryWidget.h"


TArray<UEntryWidget*> UInteraction3DWidget::InitWidget(TArray<TSubclassOf<UEntryWidget>> EntriesClass)
{
	TArray<UEntryWidget*> Entries;
	for (TSubclassOf<UEntryWidget> EntryClass : EntriesClass)
	{
		if (UEntryWidget* Entry = CreateWidget<UEntryWidget>(this, EntryClass))
		{
			Entries.Add(Entry);
			WrapBox->AddChild(Entry);
		}
	}

	return Entries;
}
