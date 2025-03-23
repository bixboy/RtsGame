#include "Player/RtsPlayerController.h"
#include "Components/RtsComponent.h"


ARtsPlayerController::ARtsPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<URtsComponent>(TEXT("SelectionComponent")))
{
	RtsComponent = Cast<URtsComponent>(SelectionComponent);
}
