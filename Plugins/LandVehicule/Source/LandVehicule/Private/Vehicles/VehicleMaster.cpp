#include "Vehicles/VehicleMaster.h"
#include "CameraVehicle.h"
#include "EnhancedInputSubsystems.h"
#include "VehiclesAnimInstance.h"
#include "GameFramework/Character.h"

// ------------------- Setup -------------------
#pragma region Setup

AVehicleMaster::AVehicleMaster()
{
    PrimaryActorTick.bCanEverTick = true;

    // Components Setup
    BaseVehicle = CreateDefaultSubobject<UStaticMeshComponent>("BaseVehicle");
    RootComponent = BaseVehicle;

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = CameraDistance;
    SpringArm->TargetOffset = FVector(0, 0, 200.f);

    MainCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    MainCamera->SetupAttachment(SpringArm);
    MainCamera->bUsePawnControlRotation = true;
}

void AVehicleMaster::BeginPlay()
{
    Super::BeginPlay();

    if (SkeletalBaseVehicle)
        AnimInstance = Cast<UVehiclesAnimInstance>(SkeletalBaseVehicle->GetAnimInstance());

    InitializeCameras();
}

void AVehicleMaster::InitializeCameras()
{
    int32 CameraIndex = 1;
    
    for (UActorComponent* Component : GetComponentsByTag(UCameraComponent::StaticClass(), "PlaceCamera"))
    {
        UCameraComponent* Cam = Cast<UCameraComponent>(Component);

        if (!Cam) continue;

        // Spawn Camera Actor
        ACameraVehicle* SpawnedCamera = GetWorld()->SpawnActor<ACameraVehicle>(
            Cam->GetComponentLocation(),
            Cam->GetComponentRotation()
        );

        if (SpawnedCamera)
        {
            Cameras.AddUnique(SpawnedCamera);

            // Attach Camera to Socket
            if (SkeletalBaseVehicle)
            {
                FString SocketNameString = FString::Printf(TEXT("Place%d"), CameraIndex);
                FName SocketAttach = FName(*SocketNameString);

                GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, SocketAttach.ToString());
                SpawnedCamera->AttachToComponent(SkeletalBaseVehicle, FAttachmentTransformRules::KeepWorldTransform, SocketAttach);
            }
            else SpawnedCamera->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

            SpawnedCamera->SetActorRotation(Cam->GetComponentRotation());

            // Initialize Animation Data
            if (AnimInstance)
                AnimInstance->TurretAngle.Add(SpawnedCamera->GetAttachParentSocketName(), FRotator::ZeroRotator);

            Cam->DestroyComponent();
            CameraIndex++;
        }
    }
}

// Possess
#pragma region Possess

    void AVehicleMaster::PossessedBy(AController* NewController)
    {
        Super::PossessedBy(NewController);
        
        if (APlayerController* PlayerController = Cast<APlayerController>(NewController))
        {
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
            {
                Subsystem->AddMappingContext(NewMappingContext, 1);
            }
        }
    }
    
    void AVehicleMaster::UnPossessed()
{
    Super::UnPossessed();

    if (CurrentDriver)
    {
        if (APlayerController* PlayerController = Cast<APlayerController>(CurrentDriver))
        {
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
            {
                //Subsystem->RemoveMappingContext(NewMappingContext);
            }
        }
        
        ReleaseRole(EVehiclePlaceType::Driver);
        CurrentDriver = nullptr;
    }
}

#pragma endregion

#pragma endregion

// ------------------- Interaction -------------------
#pragma region Interfaces

void AVehicleMaster::Interact_Implementation(APawn* PlayerInteract)
{
    IVehiclesInteractions::Interact_Implementation(PlayerInteract);
    
    APlayerController* PlayerController = Cast<APlayerController>(PlayerInteract->GetController());
    if (!PlayerController) return;

    if (!GetPlayerForRole(EVehiclePlaceType::Driver))
    {
        CurrentDriver = PlayerInteract;
        AssignRole(PlayerController, EVehiclePlaceType::Driver);
        PlayerController->Possess(this);

        PlayerController->SetViewTargetWithBlend(this, 0.1f, EViewTargetBlendFunction::VTBlend_Cubic, 0.0f, false);
        
        UE_LOG(LogTemp, Log, TEXT("Player %s now controls the vehicle as Driver."), *PlayerController->GetName());
        return;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("Driver role already occupied. Cannot interact."));
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
    {
        Subsystem->AddMappingContext(NewMappingContext, 1);

        AssignRole(PlayerController, EVehiclePlaceType::Gunner);
        SwitchToNextCamera(PlayerController);
    }
}

void AVehicleMaster::UpdateTurretRotation_Implementation(FVector2D Rotation, FName TurretName)
{
    if (!HaveTurret || Cameras.IsEmpty()) return;
    IVehiclesInteractions::UpdateTurretRotation_Implementation(Rotation, TurretName);
    
    ApplyTurretRotation(Rotation.X, Rotation.Y, TurretRotationSpeed, GetWorld()->GetTimeSeconds());

    ACameraVehicle* TempCamera = GeCameraInArray(TurretName);
    SetTurretRotation(TempCamera, GetTurretAngle(TempCamera, 1.f));
}

ACameraVehicle* AVehicleMaster::GetCurrentCameraVehicle_Implementation()
{
    return CurrentCamera;
}

void AVehicleMaster::ChangePlace_Implementation(APlayerController* PlayerController)
{
    IVehiclesInteractions::ChangePlace_Implementation(PlayerController);
    SwitchToNextCamera(PlayerController);
}

#pragma endregion

// ------------------- Role Management ---------------------
#pragma region Roles Management

    void AVehicleMaster::AssignRole(APlayerController* PlayerController, EVehiclePlaceType RoleName) 
    {
        if (!PlayerController) return;
    
        // Vérifier si le rôle est déjà occupé
        for (const FVehicleRole& TempRole : VehicleRoles)
        {
            if (TempRole.RoleName == RoleName)
            {
                return;
            }
        }
    
        VehicleRoles.Add({ PlayerController, RoleName });
    }

    void AVehicleMaster::ReleaseRole(EVehiclePlaceType RoleName)
    {
        for (int32 i = 0; i < VehicleRoles.Num(); i++)
        {
            if (VehicleRoles[i].RoleName == RoleName)
            {
                VehicleRoles.RemoveAt(i);
                return;
            }
        }
    }
    
    APlayerController* AVehicleMaster::GetPlayerForRole(EVehiclePlaceType RoleName) const
    {
        for (const FVehicleRole& TempRole : VehicleRoles)
        {
            if (TempRole.RoleName == RoleName)
            {
                return TempRole.PlayerController;
            }
        }
    
        return nullptr;
    }

    EVehiclePlaceType AVehicleMaster::GetRoleByPlayer(const APlayerController* PlayerController) const
    {
        for (const FVehicleRole& TempRole : VehicleRoles)
        {
            if (TempRole.PlayerController == PlayerController) return TempRole.RoleName;
        }
        return EVehiclePlaceType::None;
    }

#pragma endregion

// ------------------- Camera Management -------------------
#pragma region Cameras

    void AVehicleMaster::SwitchToCamera(APlayerController* PlayerController, ACameraVehicle* NewCamera)
    {
        if (!HaveTurret || Cameras.IsEmpty()) return;
    
        if (!PlayerController || !NewCamera || NewCamera->GetIsUsed()) return;
    
        // Deactivate Current Camera
        for (ACameraVehicle* TempCamera : Cameras)
        {
            if (TempCamera && TempCamera->GetIsUsed() && TempCamera->GetCameraController() == PlayerController)
            {
                TempCamera->SetIsUsed(false);
                TempCamera->SetController(nullptr);
                PlayersInVehicle.Remove(PlayerController);
            }
        }
    
        // Activate New Camera
        NewCamera->SetIsUsed(true);
        NewCamera->SetController(PlayerController);
        CurrentCamera = NewCamera;
        PlayersInVehicle.FindOrAdd(PlayerController, CurrentCamera);
    
        if (GetRoleByPlayer(PlayerController) == EVehiclePlaceType::Driver)
        {
            PlayerController->Possess(CurrentDriver);
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetPlayerForRole(EVehiclePlaceType::Driver)->GetLocalPlayer()))
            {
                Subsystem->AddMappingContext(NewMappingContext, 1);
            }
            
            ReleaseRole(EVehiclePlaceType::Driver);
            AssignRole(PlayerController, EVehiclePlaceType::Gunner);
        }
    
        PlayerController->SetViewTargetWithBlend(NewCamera, 0.1f, EViewTargetBlendFunction::VTBlend_Cubic, 0.0f, false);
        UE_LOG(LogTemp, Warning, TEXT("Switched to camera: %s for player: %s"), *NewCamera->GetName(), *PlayerController->GetName());
    }
    
    void AVehicleMaster::SwitchToNextCamera(APlayerController* PlayerController)
    {
        if (!HaveTurret || !PlayerController || Cameras.IsEmpty()) return;
    
        for (int32 i = 0; i < Cameras.Num(); i++)
        {
            ACameraVehicle* TempCamera = Cameras[i];
    
            if (TempCamera && TempCamera->GetIsUsed() && TempCamera->GetCameraController() == PlayerController)
            {
                TempCamera->SetIsUsed(false);
                TempCamera->SetController(nullptr);
    
                // Find Next Available Camera
                for (int32 j = 1; j < Cameras.Num(); j++)
                {
                    int32 NextIndex = (i + j) % Cameras.Num();
                    ACameraVehicle* NextCamera = Cameras[NextIndex];
    
                    if (NextCamera && !NextCamera->GetIsUsed())
                    {
                        SwitchToCamera(PlayerController, NextCamera);
                        return;
                    }
                }
    
                // If no camera is available, reactivate the current one
                TempCamera->SetIsUsed(true);
                TempCamera->SetController(PlayerController);
                return;
            }
        }
    
        // Default to the first unused camera
        for (ACameraVehicle* TempCamera : Cameras)
        {
            if (TempCamera && !TempCamera->GetIsUsed())
            {
                SwitchToCamera(PlayerController, TempCamera);
                return;
            }
        }
    
        UE_LOG(LogTemp, Warning, TEXT("No available camera for player: %s"), *PlayerController->GetName());
    }
    
    ACameraVehicle* AVehicleMaster::GeCameraInArray(FName TurretName)
    {
        if (Cameras.IsEmpty()) return nullptr;

        for (ACameraVehicle* TempCamera : Cameras)
        {
            if (TempCamera->GetAttachParentSocketName() == TurretName)
            {
                return TempCamera;
            }
        }
        return nullptr;
    }

#pragma endregion

// ------------------- Vehicle Controls  -------------------
#pragma region Vehicle Control

    void AVehicleMaster::TurnEngineOn(bool OnOff)
    {
        EngineOn = OnOff;
        PlaySound(SoundEngineOn);
    }
    
    void AVehicleMaster::MoveForward(float Value)
    {
        ForwardInput = Value;
    }
    
    void AVehicleMaster::MoveRight(float Value)
    {
        TurnInput = Value;
    }
    
    float AVehicleMaster::GetForwardInput() const
    {
        return ForwardInput;
    }
    
    float AVehicleMaster::GetTurnInput() const
    {
        return TurnInput;
    }

#pragma endregion

// ------------------- Turret Controls   -------------------
#pragma region Turret Control

void AVehicleMaster::ApplyTurretRotation(float DeltaYaw, float DeltaPitch, float RotationSpeed, float DeltaTime)
{
    if (CurrentCamera && SkeletalBaseVehicle)
    {
        // Mise à jour des valeurs accumulées avec les décalages donnés
        float TargetYaw = FMath::Clamp(AccumulatedYaw + DeltaYaw, -120.0f, 120.0f);
        float TargetPitch = FMath::Clamp(AccumulatedPitch + DeltaPitch, -20.0f, 20.0f);

        // Interpolation vers la nouvelle valeur
        AccumulatedYaw = FMath::FInterpTo(AccumulatedYaw, TargetYaw, DeltaTime, RotationSpeed);
        AccumulatedPitch = FMath::FInterpTo(AccumulatedPitch, TargetPitch, DeltaTime, RotationSpeed);

        // Appliquer les nouvelles rotations au composant caméra ou à la tourelle
        FRotator NewCameraRotation = CurrentCamera->GetCameraComponent()->GetRelativeRotation();
        NewCameraRotation.Pitch = FMath::Clamp(NewCameraRotation.Pitch + DeltaPitch, -80.0f, 80.0f);
    }
}

void AVehicleMaster::SetTurretRotation(ACameraVehicle* CurrenCamera, FRotator TurretAngle)
{
    if (!SkeletalBaseVehicle || !AnimInstance) return;

    FName TurretName = CurrenCamera->GetAttachParentSocketName();
    AnimInstance->UpdateTurretRotation(TurretAngle, TurretName);
}

FRotator AVehicleMaster::GetTurretAngle(ACameraVehicle* CurrenCamera, float InterpSpeed)
{
    if (!CurrentCamera || !SkeletalBaseVehicle) return CurrentAngle;
	
    CurrentAngle = FRotator(AccumulatedPitch, AccumulatedYaw, SkeletalBaseVehicle->GetSocketRotation(CurrentCamera->GetAttachParentSocketName()).Roll);
	
    return CurrentAngle;
}

#pragma endregion