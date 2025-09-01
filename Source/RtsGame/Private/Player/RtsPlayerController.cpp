#include "Player/RtsPlayerController.h"
#include "Components/RtsComponent.h"
#include "Components/RtsResourcesComponent.h"
#include "Components/WorkerComp/TaskManagerComponent.h"
#include "Net/UnrealNetwork.h"


ARtsPlayerController::ARtsPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<URtsComponent>(TEXT("SelectionComponent")))
{
	RtsComponent = Cast<URtsComponent>(SelectionComponent);

	ResourcesComponent = CreateDefaultSubobject<URtsResourcesComponent>(TEXT("ResourcesComponent"));
	TaskManager = CreateDefaultSubobject<UTaskManagerComponent>(TEXT("TaskManager"));
}

void ARtsPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (ResourcesComponent && HasAuthority())
	{
		ResourcesComponent->OnResourcesChanged.RemoveDynamic(this, &ARtsPlayerController::OnNewResources);
		ResourcesComponent->OnResourcesChanged.AddDynamic(this, &ARtsPlayerController::OnNewResources);
	}
}

void ARtsPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARtsPlayerController, CurrentTeam);
}


// ========== Teams ========== //
int ARtsPlayerController::GetPlayerTeam()
{
	return CurrentTeam;
}

void ARtsPlayerController::SetPlayerTeam(int NewPlayerTeam)
{
	if (!HasAuthority()) return;

	CurrentTeam = NewPlayerTeam;
	RtsComponent->Server_CreatSpawnPoint();
}


// ========== Resources ========== //
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
	{
		ResourcesComponent->AddResources(NewResource);	
	}
}

void ARtsPlayerController::RemoveResource(FResourcesCost RemoveResource)
{
	if (HasAuthority())
	{
		ResourcesComponent->RemoveResources(RemoveResource);	
	}
}
