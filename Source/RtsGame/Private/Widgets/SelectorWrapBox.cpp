#include "Widgets/SelectorWrapBox.h"
#include "Data/UnitsProductionDataAsset.h"
#include "Data/StructureDataAsset.h"
#include "Interfaces/BuildInterface.h"
#include "Interfaces/StorageBuildInterface.h"
#include "Interfaces/UnitProductionInterface.h"
#include "Units/BuilderUnits.h"
#include "Widgets/SelectorWidget.h"
#include "Widgets/BuildsInfo/BuildInfoBox.h"
#include "Widgets/Entries/EntryWidget.h"
#include "Widgets/Entries/UnitEntryWidget.h"
#include "Widgets/UnitsInfo/UnitsInfoBox.h"
#include "Widgets/Entries/BuildsEntry.h"
#include "Widgets/Entries/SelectionEntryWidget.h"


void USelectorWrapBox::SetupWrapBox(TArray<AActor*> SelectedActors)
{
	if (SelectedActors.IsEmpty()) return;
	
	if (!SelectorWidget)
		SelectorWidget = GetTypedOuter<USelectorWidget>();
	
	ClearChildren();

	if (SelectedActors[0]->Implements<UBuildInterface>())
	{
		SetupBuildEntries(SelectedActors);	
	}
	else
	{
		SetupUnitEntries(SelectedActors);
	}
}

void USelectorWrapBox::ClearWrapper()
{
	this->ClearChildren();
}

bool USelectorWrapBox::GetIsOpen()
{
	return SelectorWidget->GetIsOpen();
}


// ==== Units ==== //
#pragma region Units Entry

void USelectorWrapBox::SetupUnitEntries(TArray<AActor*> SelectedUnits)
{
	if (ABuilderUnits* Builder = Cast<ABuilderUnits>(SelectedUnits[0]))
	{
		CreateBuilderEntry(Builder->UnitInfo->UnitProduction.BuildsList, SelectedUnits);
		return;
	}

	if (AUnitsMaster* Unit = Cast<AUnitsMaster>(SelectedUnits[0]))
	{
		CreateUnitInfoEntry(SelectedUnits);
		return;
	}
}

void USelectorWrapBox::CreateBuilderEntry(TArray<UStructureDataAsset*> BuildList, TArray<AActor*> SelectedUnits)
{
		if (UUnitsInfoBox* WidgetInfo = CreateWidget<UUnitsInfoBox>(this, UnitsInfoClass))
		{
			WidgetInfo->SetupUnitInfos(IUnitTypeInterface::Execute_GetUnitData(SelectedUnits[0]));
			WidgetInfo->ShowBuildList(BuildList, this);
			
			this->AddChild(WidgetInfo);
		}
}

void USelectorWrapBox::CreateUnitInfoEntry(TArray<AActor*> Units)
{
	if (Units.Num() == 1)
	{
		if (UUnitsInfoBox* WidgetInfo = CreateWidget<UUnitsInfoBox>(this, UnitsInfoClass))
		{
			WidgetInfo->SetupUnitInfos(IUnitTypeInterface::Execute_GetUnitData(Units[0]));
			
			this->AddChild(WidgetInfo);
		}
	}
	else
	{
		TMap<UUnitsProductionDataAsset*, FGroupedActors> GroupedSelection;
		for (AActor* Actor : Units)
		{
			AUnitsMaster* UnitsMaster = Cast<AUnitsMaster>(Actor);
			if (!UnitsMaster || !UnitsMaster->UnitInfo) continue;

			UUnitsProductionDataAsset* DataAsset = UnitsMaster->UnitInfo;
			GroupedSelection.FindOrAdd(DataAsset).Actors.Add(Actor);
		}
	
		CreateUnitsList(GroupedSelection);
	}
}

void USelectorWrapBox::CreateUnitsList(TMap<UUnitsProductionDataAsset*, FGroupedActors> UnitsMap)
{
	if (UnitsMap.IsEmpty()) return;
	
	ClearChildren();

	for (const TPair<UUnitsProductionDataAsset*, FGroupedActors>& Pair : UnitsMap)
	{
		UUnitsProductionDataAsset* DataAsset = Pair.Key;
		const FGroupedActors& Group = Pair.Value;

		if (DataAsset)
		{
			if (USelectionEntryWidget* SelectionWidget = CreateWidget<USelectionEntryWidget>(GetWorld(), UnitListEntryClass))
			{
				SelectionWidget->InitializeEntry(DataAsset);
				SelectionWidget->SetupEntry(Group.Actors.Num());
				
				AddChild(SelectionWidget);
				
				AddDelegateToolTip(SelectionWidget);
			}
		}
	}
}

#pragma endregion


// ==== Build ==== //
#pragma region Builds Entry

void USelectorWrapBox::SetupBuildEntries(TArray<AActor*> SelectedBuilds)
{
	if (SelectedBuilds.IsEmpty()) return;

	if (UBuildInfoBox* Widget = CreateWidget<UBuildInfoBox>(this, BuildsInfoClass))
	{
		BuildInfoBox = Widget;
		
		Widget->SetupBuildInfo(SelectedBuilds, this);

		this->AddChild(Widget);
	}
}

#pragma endregion


// ==== Tool Tip ==== //
void USelectorWrapBox::AddDelegateToolTip(UEntryWidget* Entry)
{
	Entry->OnEntryHovered.RemoveDynamic(this, &USelectorWrapBox::ShowToolTip);
	Entry->OnEntryUnHovered.RemoveDynamic(this, &USelectorWrapBox::HideToolTip);
        	
	Entry->OnEntryHovered.AddDynamic(this, &USelectorWrapBox::ShowToolTip);
	Entry->OnEntryUnHovered.AddDynamic(this, &USelectorWrapBox::HideToolTip);
}

void USelectorWrapBox::ShowToolTip(UDataAsset* Data)
{
	if (SelectorWidget)
	{
		SelectorWidget->ShowToolTip(Data);
	}
}

void USelectorWrapBox::HideToolTip()
{
	if (SelectorWidget)
	{
		SelectorWidget->HideToolTip();
	}
}
