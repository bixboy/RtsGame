#include "CustomPlayerController.h"

#include "CameraVehicle.h"
#include "EnhancedInputSubsystems.h"
#include "Net/UnrealNetwork.h"
#include "Vehicles/VehicleMaster.h"

// ------------------- Replication -------------------
#pragma region Replication

void ACustomPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ACustomPlayerController, bInVehicle);
    DOREPLIFETIME(ACustomPlayerController, CurrentVehicle);
    DOREPLIFETIME(ACustomPlayerController, CurrentCamera);
}

#pragma endregion

// ------------------- Mapping Context -------------------
#pragma region Mapping Context

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

#pragma endregion

// ------------------- Vehicle and Camera Switching -------------------
#pragma region Vehicle and Camera Switching

void ACustomPlayerController::Server_ChangeCamera_Implementation(AVehicleMaster* Vehicle)
{
    if (Vehicle)
    {
        CurrentCamera = Execute_ChangePlace(Vehicle, this);
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
        {
            CurrentVehicle = Vehicle;
        }
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

#pragma endregion

// ------------------- Turret Rotation -------------------
#pragma region Turret Rotation

void ACustomPlayerController::RotateVehicleTurret(FVector2D NewRotation)
{
    Server_RotateVehicleTurret(NewRotation);
}

void ACustomPlayerController::Server_RotateVehicleTurret_Implementation(const FVector2D NewRotation)
{
    if (!CurrentVehicle || !CurrentCamera || !CurrentCamera->GetIsTurret())
        return;

    CurrentVehicle->OnTurretRotate(NewRotation, CurrentCamera);
}

#pragma endregion

// ------------------- Getters -------------------
#pragma region Getters

ACameraVehicle* ACustomPlayerController::GetCurrentCamera()
{
    return CurrentCamera;
}

AVehicleMaster* ACustomPlayerController::GetCurrentVehicle()
{
    return CurrentVehicle;
}

#pragma endregion

// ------------------- OnRep Functions -------------------
#pragma region OnRep Functions

void ACustomPlayerController::OnRep_CurrentVehicle()
{
    GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, TEXT("OnRep_CurrentVehicle: Updated in Blueprint!"));
}

void ACustomPlayerController::OnRep_bInVehicle()
{
    GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, TEXT("OnRep_bInVehicle: Updated in Blueprint!"));
}

#pragma endregion