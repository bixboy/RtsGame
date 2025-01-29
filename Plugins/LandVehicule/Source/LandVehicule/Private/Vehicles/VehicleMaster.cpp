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
    int32 TurretIndex = 0;
    for (UActorComponent* Component : GetComponentsByTag(UStaticMeshComponent::StaticClass(), "SmTurrets"))
    {
        UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(Component);
        if (MeshComp)
        {
            SmTurrets.AddUnique(MeshComp);
        }
    }
    
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
            else if (BaseVehicle)
            {
                FString SocketNameString = FString::Printf(TEXT("Place%d"), TurretIndex);
                GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, SocketNameString);
                
                if (!SmTurrets.IsEmpty() && SmTurrets.IsValidIndex(TurretIndex))
                {
                    SpawnedCamera->AttachToComponent(SmTurrets[TurretIndex], FAttachmentTransformRules::KeepWorldTransform);
                    TurretIndex++;
                }
                
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
            ReleaseRole(EVehiclePlaceType::Driver);
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
        AssignRole(PlayerInteract, EVehiclePlaceType::Driver);
        
        PlayerController->Possess(this);
        PlayerController->SetViewTargetWithBlend(this, 0.1f, EViewTargetBlendFunction::VTBlend_Cubic, 0.0f, false);
        
        return;
    }
    else if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
    {
        Subsystem->AddMappingContext(NewMappingContext, 1);

        AssignRole(PlayerInteract, EVehiclePlaceType::Gunner);
        SwitchToNextCamera(PlayerInteract);
    }
}

void AVehicleMaster::UpdateTurretRotation_Implementation(FVector2D Rotation, FName TurretName)
{
    IVehiclesInteractions::UpdateTurretRotation_Implementation(Rotation, TurretName);
    
    if (!HaveTurret || Cameras.IsEmpty()) return;

    ApplyTurretRotation(Rotation.X, Rotation.Y, TurretRotationSpeed, GetWorld()->GetTimeSeconds());

    ACameraVehicle* TempCamera = GeCameraInArray(TurretName);
    if (!TempCamera)
    {
        UE_LOG(LogTemp, Error, TEXT("UpdateTurretRotation: TempCamera is NULL for TurretName: %s"), *TurretName.ToString());
        return;
    }

    SetTurretRotation(TempCamera, GetTurretAngle(TempCamera, 1.f));
}

ACameraVehicle* AVehicleMaster::GetCurrentCameraVehicle_Implementation()
{
    return CurrentCamera;
}

void AVehicleMaster::ChangePlace_Implementation(APawn* Player)
{
    IVehiclesInteractions::ChangePlace_Implementation(Player);
    SwitchToNextCamera(Player);
}

#pragma endregion

// ------------------- Role Management ---------------------
#pragma region Roles Management

    void AVehicleMaster::AssignRole(APawn* Player, EVehiclePlaceType RoleName) 
    {
        if (!Player) return;
    
        // Vérifier si le rôle est déjà occupé
        for (const FVehicleRole& TempRole : VehicleRoles)
        {
            if (TempRole.RoleName == RoleName)
            {
                return;
            }
        }
    
        VehicleRoles.Add({ Player, RoleName });
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
    
    APawn* AVehicleMaster::GetPlayerForRole(EVehiclePlaceType RoleName) const
    {
        for (const FVehicleRole& TempRole : VehicleRoles)
        {
            if (TempRole.RoleName == RoleName)
            {
                return TempRole.Player;
            }
        }
    
        return nullptr;
    }

    EVehiclePlaceType AVehicleMaster::GetRoleByPlayer(const APawn* Player) const
    {
        for (const FVehicleRole& TempRole : VehicleRoles)
        {
            if (TempRole.Player == Player) return TempRole.RoleName;
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
    
        if (PlayerController->GetPawn() == CurrentDriver)
        {
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
            {
                Subsystem->AddMappingContext(NewMappingContext, 1);
            }

            ReleaseRole(EVehiclePlaceType::Driver);
            CurrentDriver = nullptr;
            AssignRole(PlayerController->GetPawn(), EVehiclePlaceType::Gunner);
        }
    
        PlayerController->SetViewTargetWithBlend(NewCamera, 0.1f, EViewTargetBlendFunction::VTBlend_Cubic, 0.0f, false);
    }
    
    void AVehicleMaster::SwitchToNextCamera(APawn* Player)
    {
        if (!HaveTurret || !Player || Cameras.IsEmpty()) return;
    
        APlayerController* PlayerController = Cast<APlayerController>(Player->GetController());
        if (!PlayerController) return;

        if (this == Player)
        {
            PlayerController->Possess(CurrentDriver);
            PlayerController = Cast<APlayerController>(CurrentDriver->GetController());
        }
    
        // Parcours des caméras pour trouver la caméra actuelle du joueur
        for (int32 i = 0; i < Cameras.Num(); i++)
        {
            ACameraVehicle* TempCamera = Cameras[i];
            if (TempCamera && TempCamera->GetIsUsed() && TempCamera->GetCameraController() == PlayerController)
            {
                // Libère la caméra actuelle
                TempCamera->SetIsUsed(false);
                TempCamera->SetController(nullptr);
    
                // Vérifie si c'est la dernière caméra dans la liste
                if (i == Cameras.Num() - 1)
                {
                    if (!CurrentDriver && MainCamera)
                    {
                        SwitchToMainCam(PlayerController);
                        return;
                    }
                    else
                    {
                        // Si pas de MainCamera, revient à la première caméra
                        if (Cameras[0])
                        {
                            SwitchToCamera(PlayerController, Cameras[0]);
                        }
                        return;
                    }
                }
    
                // Passe à la prochaine caméra disponible
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
    
                // Si aucune caméra n'est disponible, réactive la caméra actuelle
                TempCamera->SetIsUsed(true);
                TempCamera->SetController(PlayerController);
                return;
            }
        }
    
        // Si aucune caméra n'est assignée, trouve une caméra inutilisée par défaut
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

    void AVehicleMaster::SwitchToMainCam(APlayerController* PlayerController)
    {
        Execute_Interact(this, PlayerController->GetCharacter());
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
    if (CurrentCamera)
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

void AVehicleMaster::SetTurretRotation(ACameraVehicle* Camera, FRotator TurretAngle)
{
    if (SkeletalBaseVehicle && AnimInstance)
    {
        FName TurretName = Camera->GetAttachParentSocketName();
        AnimInstance->UpdateTurretRotation(TurretAngle, TurretName);   
    }
    else if (BaseVehicle)
    {
        SmTurrets[Cameras.IndexOfByKey(CurrentCamera)]->SetRelativeRotation(FRotator(0.f, TurretAngle.Yaw, TurretAngle.Pitch));
    }
}

FRotator AVehicleMaster::GetTurretAngle(ACameraVehicle* CurrenCamera, float InterpSpeed)
{
    if (!CurrentCamera) return CurrentAngle;

    if (SkeletalBaseVehicle)
    {
        CurrentAngle = FRotator(AccumulatedPitch, AccumulatedYaw, SkeletalBaseVehicle->GetSocketRotation(CurrentCamera->GetAttachParentSocketName()).Roll);
    }
    else if (BaseVehicle && !SmTurrets.IsEmpty())
    {
        CurrentAngle = FRotator(AccumulatedPitch, AccumulatedYaw, SmTurrets[Cameras.IndexOfByKey(CurrentCamera)]->GetComponentRotation().Roll);
    }
	
    return CurrentAngle;
}

#pragma endregion