#include "Player/RtsSelectionBox.h"
#include "Interfaces/FactionsInterface.h"
#include "Player/RtsPlayerController.h"


ARtsSelectionBox::ARtsSelectionBox()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ARtsSelectionBox::BeginPlay()
{
	Super::BeginPlay();

	if (PlayerController)
	{
		RtsController = Cast<ARtsPlayerController>(PlayerController);
	}
}

void ARtsSelectionBox::OnBoxCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (RtsController)
	{
		if (OtherActor->Implements<UFactionsInterface>())
		{
			if (IFactionsInterface::Execute_GetTeam(OtherActor) == RtsController->GetPlayerTeam() || IFactionsInterface::Execute_GetTeam(OtherActor) == -1)
			{
				Super::OnBoxCollisionBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep,SweepResult);
			}
			
			return;
		}	
	}
	
	Super::OnBoxCollisionBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep,SweepResult);
}


