#include "Player/RtsPlayerController.h"
#include "Components/RtsComponent.h"
#include "Components/RtsResourcesComponent.h"


ARtsPlayerController::ARtsPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<URtsComponent>(TEXT("SelectionComponent")))
{
	RtsComponent = Cast<URtsComponent>(SelectionComponent);

	ResourcesComponent = CreateDefaultSubobject<URtsResourcesComponent>(TEXT("ResourcesComponent"));

	ResourcesComponent->OnResourcesChanged.AddDynamic(this, &ARtsPlayerController::OnNewResources);
}

void ARtsPlayerController::OnNewResources(const FResourcesCost& NewResources)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, "Resources : " + NewResources.Woods);
}
