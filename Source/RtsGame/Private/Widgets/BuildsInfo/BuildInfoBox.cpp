#include "Widgets/BuildsInfo/BuildInfoBox.h"

#include "Components/RtsResourcesComponent.h"
#include "Data/UnitsProductionDataAsset.h"
#include "Components/WrapBox.h"
#include "Structures/ResourceDepot.h"
#include "Structures/UnitsProduction/UnitProduction.h"
#include "Widgets/SelectorWrapBox.h"
#include "Widgets/BuildsInfo/BuildResourceInfo.h"
#include "Widgets/Entries/UnitEntryWidget.h"


void UBuildInfoBox::SetupBuildInfo(TArray<AActor*> Builds, USelectorWrapBox* Owner)
{
	if (Builds.IsEmpty()) return;

	if (!OwnerWidget)
		OwnerWidget = Owner;

	WrapBox->ClearChildren();
	
	if (AResourceDepot* Build = Cast<AResourceDepot>(Builds[0]))
	{
		WrapBox->SetVisibility(ESlateVisibility::Collapsed);
		BuildResource->SetVisibility(ESlateVisibility::Visible);
		
		CreateResourceEntry(Builds);
		return;
	}

	if (AUnitProduction* Build = Cast<AUnitProduction>(Builds[0]))
	{
		WrapBox->SetVisibility(ESlateVisibility::Visible);
		BuildResource->SetVisibility(ESlateVisibility::Collapsed);
		
		CreateUnitEntry(Builds);
	}
}

// ==== Resource Entry ==== //
void UBuildInfoBox::CreateResourceEntry(TArray<AActor*> SelectedBuilds)
{
	if (SelectedBuilds.IsEmpty()) return;
	
	FResourcesCost TotalResources;
	for (AActor* Storage : SelectedBuilds)
	{
		if (Storage->Implements<UStorageBuildInterface>())
		{
			FResourcesCost Res = IStorageBuildInterface::Execute_GetResource(Storage);
			TotalResources += Res;

			if (AResourceDepot* Build = Cast<AResourceDepot>(Storage))
			{
				Build->OnStorageUpdated.RemoveDynamic(this, &UBuildInfoBox::UpdateResources);
				Build->OnStorageUpdated.AddDynamic(this, &UBuildInfoBox::UpdateResources);
			}
		}
	}

	BuildResource->UpdateResources(TotalResources);
}

void UBuildInfoBox::UpdateResources(FResourcesCost ResourcesCost)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, "Build Resources:");
	BuildResource->UpdateResources(ResourcesCost);
}


// ==== Unit Prod Entry ==== //
void UBuildInfoBox::CreateUnitEntry(TArray<AActor*> ProductionBuilds)
{
	bool bShowCount = (ProductionBuilds.Num() == 1);

	TMap<UUnitsProductionDataAsset*, int32> ProductionCounts;
	TMap<UUnitsProductionDataAsset*, AActor*> DataToBuildMap;

	TSet<UUnitsProductionDataAsset*> UniqueDataAssets;
	
	for (AActor* Build : ProductionBuilds)
	{
		if (!Build) continue;

		TArray<UUnitsProductionDataAsset*> Available = IUnitProductionInterface::Execute_GetProductionList(Build);

		for (UUnitsProductionDataAsset* Data : Available)
		{
			if (!Data) continue;

			if (!UniqueDataAssets.Contains(Data))
			{
				UniqueDataAssets.Add(Data);
				DataToBuildMap.Add(Data, Build);
			}
		}

		if (bShowCount)
		{
			TArray<UUnitsProductionDataAsset*> Queue = IUnitProductionInterface::Execute_GetUnitsProduction(Build);
			for (UUnitsProductionDataAsset* ProdData : Queue)
			{
				if (ProdData)
				{
					ProductionCounts.FindOrAdd(ProdData)++;
				}
			}
		}
	}

	for (UUnitsProductionDataAsset* Data : UniqueDataAssets)
	{
		if (!Data) continue;

		int32 NumInQueue = ProductionCounts.Contains(Data) ? ProductionCounts[Data] : 0;
    	
		AActor* MatchingBuild = DataToBuildMap.Contains(Data) ? DataToBuildMap[Data] : nullptr;

		if (UUnitEntryWidget* UnitWidget = CreateWidget<UUnitEntryWidget>(GetWorld(), UnitsProdEntryClass))
		{
			UnitWidget->InitializeEntry(Data);
			UnitWidget->SetupEntry(MatchingBuild, NumInQueue, bShowCount);

			UnitEntryList.Add(UnitWidget);

			OwnerWidget->AddDelegateToolTip(UnitWidget);
        	
			WrapBox->AddChild(UnitWidget);
		}
	}

	if (bShowCount)
	{
		if (bShowCount)
		{
			AUnitProduction* Prod = Cast<AUnitProduction>(ProductionBuilds[0]);
			if (Prod && !Prod->OnUnitProduced.IsAlreadyBound(this, &UBuildInfoBox::OnUnitProduced))
			{
				Prod->OnUnitProduced.AddDynamic(this, &UBuildInfoBox::OnUnitProduced);
			}	
		}
	}
}

void UBuildInfoBox::OnUnitProduced(AActor* Actor)
{
	if (!OwnerWidget->GetIsOpen()) return;
	
	for (UUnitEntryWidget* UnitWidget : UnitEntryList)
	{
		if (UnitWidget)
		{
			UnitWidget->UpdateEntry();
		}
	}
}
