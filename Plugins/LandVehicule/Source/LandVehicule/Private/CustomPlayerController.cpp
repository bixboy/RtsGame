#include "CustomPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Net/UnrealNetwork.h"
#include "Vehicles/VehicleMaster.h"

void ACustomPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACustomPlayerController, bInVehicle);
	DOREPLIFETIME(ACustomPlayerController, CurrentVehicle);
}

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

void ACustomPlayerController::Server_ChangeCamera_Implementation(AVehicleMaster* Vehicle)
{
	if (Vehicle)
	{
		Execute_ChangePlace(Vehicle, this);
	}
}

void ACustomPlayerController::EnterInVehicle(AVehicleMaster* Vehicle)
{
	if (Vehicle)
	{
		Server_EnterInVehicle(Vehicle);
	}
}

void ACustomPlayerController::Server_EnterInVehicle_Implementation(AVehicleMaster* Vehicle)
{
	if (Vehicle)
	{
		bInVehicle = Execute_Interact(Vehicle, this);

		if (bInVehicle)
			CurrentVehicle = Vehicle;
	}
}

void ACustomPlayerController::Server_OutOfVehicle_Implementation(AVehicleMaster* Vehicle)
{
	if (Vehicle)
	{
		Execute_OutOfVehicle(Vehicle, this);
		
		CurrentVehicle = nullptr;
		bInVehicle = false;
	}
}

void ACustomPlayerController::OnRep_CurrentVehicle()
{
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, TEXT("OnRep_CurrentVehicle: Updated in Blueprint!"));
}

void ACustomPlayerController::OnRep_bInVehicle()
{
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, TEXT("OnRep_bInVehicle: Updated in Blueprint!"));
}

