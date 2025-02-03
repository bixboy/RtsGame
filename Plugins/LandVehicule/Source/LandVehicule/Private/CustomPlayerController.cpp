#include "CustomPlayerController.h"
#include "EnhancedInputSubsystems.h"

void ACustomPlayerController::Client_AddMappingContext_Implementation(const UInputMappingContext* NewMappingContext) const
{
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(NewMappingContext, 1);
	}
}

void ACustomPlayerController::Client_RemoveMappingContext_Implementation(const UInputMappingContext* MappingContextToRemove) const
{
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->RemoveMappingContext(MappingContextToRemove);
	}
}
