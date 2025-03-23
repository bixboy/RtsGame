#include "Widgets/BuildsEntry.h"

#include "Components/RtsComponent.h"
#include "Data/DataRts.h"
#include "Data/StructureDataAsset.h"
#include "Player/RtsPlayerController.h"
#include "Structures/StructureBase.h"
#include "Widget/CustomButtonWidget.h"


void UBuildsEntry::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	verify((PlayerController = Cast<ARtsPlayerController>(GetWorld()->GetFirstPlayerController())));

	if (BuildButton)
		BuildButton->OnButtonClicked.AddDynamic(this, &UBuildsEntry::OnBuildSelected);
}

void UBuildsEntry::InitEntry(UStructureDataAsset* DataAsset)
{
	FStructure BuildData = DataAsset->Structure;
		
	BuildButton->SetButtonTexture(BuildData.Image);
	BuildButton->SetButtonText(FText::FromString(BuildData.Name));
	BuildClass = BuildData.BuildClass;
}

void UBuildsEntry::OnBuildSelected(UCustomButtonWidget* Button, int Index)
{
	if (!BuildClass) return;

	PlayerController->RtsComponent->ChangeBuildClass(BuildClass);
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, "Unit Selected : " + BuildClass->GetName());
}
