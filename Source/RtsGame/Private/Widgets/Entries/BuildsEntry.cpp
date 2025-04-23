#include "Widgets/Entries/BuildsEntry.h"
#include "Components/RtsComponent.h"
#include "Data/DataRts.h"
#include "Data/StructureDataAsset.h"
#include "Player/RtsPlayerController.h"
#include "Structures/StructureBase.h"
#include "Widget/CustomButtonWidget.h"


void UBuildsEntry::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (CustomButton)
	{
		CustomButton->OnButtonClicked.AddDynamic(this, &UBuildsEntry::OnBuildSelected);
	}
}

void UBuildsEntry::InitializeEntry(UDataAsset* NewDataAsset)
{
	Super::InitializeEntry(NewDataAsset);

	BuildData = Cast<UStructureDataAsset>(NewDataAsset);
	
	CustomButton->SetButtonTexture(BuildData->Structure.Image);
	CustomButton->SetButtonText(FText::FromString(BuildData->Structure.Name));
}

void UBuildsEntry::OnBuildSelected(UCustomButtonWidget* Button, int Index)
{
	if (!BuildData || !BuildData->Structure.StructureMesh || !BuildData->Structure.BuildClass) return;

	PlayerController->RtsComponent->ChangeBuildClass(BuildData->Structure);
	
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, "Unit Selected : " + BuildData->Structure.BuildClass->GetName());
}
