#include "Player/RtsPlayerController.h"
#include "Components/RtsComponent.h"
#include "Components/RtsResourcesComponent.h"


ARtsPlayerController::ARtsPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<URtsComponent>(TEXT("SelectionComponent")))
{
	RtsComponent = Cast<URtsComponent>(SelectionComponent);

	ResourcesComponent = CreateDefaultSubobject<URtsResourcesComponent>(TEXT("ResourcesComponent"));
	
}

void ARtsPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (ResourcesComponent)
	{
		ResourcesComponent->OnResourcesChanged.RemoveDynamic(this, &ARtsPlayerController::OnNewResources);
		ResourcesComponent->OnResourcesChanged.AddDynamic(this, &ARtsPlayerController::OnNewResources);
	}
}

void ARtsPlayerController::OnNewResources(const FResourcesCost& NewResources)
{
	if (RtsComponent)
	{
		RtsComponent->Client_UpdateResourceValue(NewResources);
	}
}

void ARtsPlayerController::AddResource(FResourcesCost NewResource)
{
	if (HasAuthority())
		ResourcesComponent->AddResources(NewResource);
}

void ARtsPlayerController::RemoveResource(FResourcesCost RemoveResource)
{
	if (HasAuthority())
		ResourcesComponent->RemoveResources(RemoveResource);
}
