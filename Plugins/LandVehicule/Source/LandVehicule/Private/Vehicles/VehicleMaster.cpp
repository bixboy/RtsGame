#include "Vehicles/VehicleMaster.h"
#include "CameraVehicle.h"
#include "CustomPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "VehiclesAnimInstance.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

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

// Need Clear
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
            FTurrets Turret;
            Turret.CameraVehicle = SpawnedCamera;
            Turrets.AddUnique(Turret);

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

void AVehicleMaster::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AVehicleMaster, CurrentDriver);
    DOREPLIFETIME(AVehicleMaster, VehicleRoles);
}

// Possess
#pragma region Possess

    void AVehicleMaster::PossessedBy(AController* NewController)
    {
        Super::PossessedBy(NewController);
        
        if (APlayerController* PlayerController = Cast<APlayerController>(NewController))
        {
            if (ACustomPlayerController* CustomPC = Cast<ACustomPlayerController>(PlayerController))
            {
                SetOwner(PlayerController);
                CustomPC->Client_AddMappingContext(NewMappingContext);
            }
        }
    }

#pragma endregion

#pragma endregion 

// ------------------- Interaction -------------------
#pragma region Interfaces

bool AVehicleMaster::Interact_Implementation(ACustomPlayerController* PlayerController)
{
    IVehiclesInteractions::Interact_Implementation(PlayerController);

    APawn* Player = PlayerController->GetPawn();
    
    if (PlacesNumber == CurrentPlace || GetRoleByPlayer(Player) == EVehiclePlaceType() || !HasAuthority()) return false;
    
    if (!GetPlayerForRole(EVehiclePlaceType::Driver))
    {
        CurrentDriver = Player;
        AssignRole(Player, EVehiclePlaceType::Driver);

        PlayerController->Possess(this);
        PlayerController->SetViewTargetWithBlend(this, 0.1f, EViewTargetBlendFunction::VTBlend_Cubic, 0.0f, false);

        CurrentPlace ++;
        return true;
    }
    
    if (!HaveTurret) return false;
    
    AssignRole(Player, EVehiclePlaceType::Gunner);
    SwitchToNextCamera(PlayerController);

    if (ACustomPlayerController* CustomPC = Cast<ACustomPlayerController>(PlayerController))
    {
        CustomPC->Client_AddMappingContext(NewMappingContext);
    }

    CurrentPlace++;
    return true;
}

void AVehicleMaster::OutOfVehicle_Implementation(ACustomPlayerController* PlayerController)
{
    IVehiclesInteractions::OutOfVehicle_Implementation(PlayerController);

    APawn* Player = PlayerController->GetPawn();
    if (!Player) return;
    
    if (this == Player)
    {
        if (CurrentDriver)
        {
            CurrentDriver->SetActorTransform(Player->GetActorTransform());
            
            PlayerController->Possess(CurrentDriver);
            PlayerController->Client_RemoveMappingContext(NewMappingContext);
            
            ReleaseRole(CurrentDriver);
            CurrentDriver = nullptr;
            CurrentPlace --;
        }
    }
    else
    {
        if (!CurrentCamera.CameraVehicle) return;
            
        PlayerController->SetViewTargetWithBlend(Player, 0.1f, EViewTargetBlendFunction::VTBlend_Cubic, 0.0f, false);
        ReleaseRole(Player);
        CurrentCamera.CameraVehicle->SetIsUsed(false);
        
        PlayerController->Client_RemoveMappingContext(NewMappingContext);
        
        CurrentPlace --;
    }
}

void AVehicleMaster::ChangePlace_Implementation(ACustomPlayerController* Player)
{
    IVehiclesInteractions::ChangePlace_Implementation(Player);

    SwitchToNextCamera(Player);
}

void AVehicleMaster::UpdateTurretRotation_Implementation(FVector2D Rotation, FName TurretName)
{
    IVehiclesInteractions::UpdateTurretRotation_Implementation(Rotation, TurretName);
    
    if (!HaveTurret || Turrets.IsEmpty()) return;

    GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, "ouou");
    ApplyTurretRotation(Rotation.X, Rotation.Y, TurretRotationSpeed, GetWorld()->GetTimeSeconds());
    SetTurretRotation(CurrentCamera.CameraVehicle, GetTurretAngle());
}

ACameraVehicle* AVehicleMaster::GetCurrentCameraVehicle_Implementation()
{
    return CurrentCamera.CameraVehicle;
}

#pragma endregion

// ------------------- Role Management   ---------------------
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

    void AVehicleMaster::ReleaseRole(APawn* Player)
    {
        FVehicleRole RoleToRemove;
        for (FVehicleRole TempRole : VehicleRoles)
        {
            if (TempRole.Player == Player)
            {
                RoleToRemove = TempRole;
            }
        }
        VehicleRoles.Remove(RoleToRemove);
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

    void AVehicleMaster::SwitchToCamera(APlayerController* PlayerController, const FTurrets& NewCamera)
    {
        if (!HaveTurret || Turrets.IsEmpty()) return;
        if (!PlayerController || !NewCamera.CameraVehicle || NewCamera.CameraVehicle->GetIsUsed()) return;
    
        if (!HasAuthority())  
        {
            Server_SwitchToCamera(PlayerController, NewCamera);
            return;
        }
    
        // Désactiver la caméra actuelle
        if (FTurrets* CurrentTurret = Turrets.FindByPredicate([PlayerController](const FTurrets& Turret)
            {
                return Turret.CameraVehicle && Turret.CameraVehicle->GetIsUsed() &&
                       Turret.CameraVehicle->GetCameraController() == PlayerController;
            }))
        {
            CurrentTurret->CameraVehicle->SetIsUsed(false);
            CurrentTurret->CameraVehicle->SetController(nullptr);
            PlayersInVehicle.Remove(PlayerController);
        }

        // Activer la nouvelle caméra
        NewCamera.CameraVehicle->SetIsUsed(true);
        NewCamera.CameraVehicle->SetController(PlayerController);

        // Trouver l'ancienne caméra et copier les valeurs d'orientation
        if (FTurrets* OldTurret = Turrets.FindByPredicate([this](const FTurrets& Turret)
            {
                return Turret.CameraVehicle == CurrentCamera.CameraVehicle;
            }))
        {
            OldTurret->AccumulatedPitch = CurrentCamera.AccumulatedPitch;
            OldTurret->AccumulatedYaw = CurrentCamera.AccumulatedYaw;
        }

        // Mettre à jour la caméra actuelle
        Client_SwitchToCamera(PlayerController, NewCamera);
        PlayersInVehicle.FindOrAdd(PlayerController, CurrentCamera.CameraVehicle);

        // Si le joueur contrôlait le véhicule en tant que conducteur
        if (PlayerController->GetPawn() == CurrentDriver)
        {
            if (ACustomPlayerController* CustomPC = Cast<ACustomPlayerController>(PlayerController))
            {
                CustomPC->Client_AddMappingContext(NewMappingContext);
            }

            ReleaseRole(CurrentDriver);
            AssignRole(CurrentDriver, EVehiclePlaceType::Gunner);
            CurrentDriver = nullptr;
        }

        // Changer la vue du joueur vers la nouvelle caméra
        PlayerController->SetViewTargetWithBlend(NewCamera.CameraVehicle, 0.1f, EViewTargetBlendFunction::VTBlend_Cubic, 0.0f, false);
    }

    void AVehicleMaster::Server_SwitchToCamera_Implementation(APlayerController* PlayerController, const FTurrets& NewCamera)
    {
        SwitchToCamera(PlayerController, NewCamera);
    }

    void AVehicleMaster::Client_SwitchToCamera_Implementation(APlayerController* PlayerController, const FTurrets& NewCamera)
    {
        if (!NewCamera.CameraVehicle) return;

        CurrentCamera = NewCamera;
        GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, "New Camera = " + CurrentCamera.CameraVehicle->GetName());
    }
    
    void AVehicleMaster::SwitchToNextCamera(APlayerController* PlayerController)
    {
        if (!HaveTurret || !PlayerController || Turrets.IsEmpty()) return;
    
        APawn* Player = PlayerController->GetPawn();
        if (!Player) return;

        if (this == Player)
        {
            PlayerController->Possess(CurrentDriver);
            PlayerController = Cast<APlayerController>(CurrentDriver->GetController());
        }
    
        // Parcours des caméras pour trouver la caméra actuelle du joueur
        for (int32 i = 0; i < Turrets.Num(); i++)
        {
            FTurrets Turret = Turrets[i];
            if (Turret.CameraVehicle && Turret.CameraVehicle->GetIsUsed() && Turret.CameraVehicle->GetCameraController() == PlayerController)
            {
                // Libère la caméra actuelle
                Turret.CameraVehicle->SetIsUsed(false);
                Turret.CameraVehicle->SetController(nullptr);
    
                // Vérifie si c'est la dernière caméra dans la liste
                if (i == Turrets.Num() - 1 || !GetAvailableCamera(i).CameraVehicle)
                {
                    GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, MainCamera->GetName());
                    if (!CurrentDriver && MainCamera)
                    {
                        SwitchToMainCam(PlayerController);
                        return;
                    }
                }
                
                // Passe à la prochaine caméra disponible
                FTurrets NextAvailableCamera = GetAvailableCamera(i);
                if (NextAvailableCamera.CameraVehicle)
                {
                    SwitchToCamera(PlayerController, NextAvailableCamera);
                    return;
                }
    
                // Si aucune caméra n'est disponible, réactive la caméra actuelle
                Turret.CameraVehicle->SetIsUsed(true);
                Turret.CameraVehicle->SetController(PlayerController);
                return;
            }
        }
    
        // Si aucune caméra n'est assignée, trouve une caméra inutilisée par défaut
        for (FTurrets Turret : Turrets)
        {
            if (Turret.CameraVehicle && !Turret.CameraVehicle->GetIsUsed())
            {
                SwitchToCamera(PlayerController, Turret);
                return;
            }
        }
    
        UE_LOG(LogTemp, Warning, TEXT("No available camera for player: %s"), *PlayerController->GetName());
    }

    void AVehicleMaster::SwitchToMainCam(APlayerController* PlayerController)
    {
        if (!CurrentDriver)
        {
            CurrentDriver = PlayerController->GetCharacter();
            AssignRole(PlayerController->GetCharacter(), EVehiclePlaceType::Driver);
            
            PlayerController->Possess(this);
            PlayerController->SetViewTargetWithBlend(this, 0.1f, EViewTargetBlendFunction::VTBlend_Cubic, 0.0f, false);
        }
    }

    ACameraVehicle* AVehicleMaster::GetAttachedCamera(FName ParentName)
    {
        if (Turrets.IsEmpty()) return nullptr;

        for (FTurrets Turret : Turrets)
        {
            if (Turret.CameraVehicle->GetAttachParentSocketName() == ParentName)
            {
                return Turret.CameraVehicle;
            }
        }
        return nullptr;
    }

    int AVehicleMaster::GetCameraIndex(const ACameraVehicle* CameraVehicle)
    {
        for (int32 i = 0; i < Turrets.Num(); i++)
        {
            if (Turrets[i].CameraVehicle == CameraVehicle)
            {
                return i;
            }
        }
        return 0;
    }

    FTurrets AVehicleMaster::GetAvailableCamera(int startIndex)
    {
        // Passe à la prochaine caméra disponible
        for (int32 j = 1; j < Turrets.Num(); j++)
        {
            int32 NextIndex = (startIndex + j) % Turrets.Num();
            FTurrets NextCamera = Turrets[NextIndex];
        
            if (NextCamera.CameraVehicle && !NextCamera.CameraVehicle->GetIsUsed())
            {
                return NextCamera;
            }
        }
        return FTurrets();
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
    if (CurrentCamera.CameraVehicle)
    {
        const float Pitch = CurrentCamera.AccumulatedPitch;
        const float Yaw = CurrentCamera.AccumulatedYaw;
        
        // Mise à jour valeurs
        float TargetYaw = FMath::Clamp(Yaw + DeltaYaw, -120.0f, 120.0f);
        float TargetPitch = FMath::Clamp(Pitch + DeltaPitch, -20.0f, 20.0f);

        // Interpolation
        CurrentCamera.AccumulatedYaw = FMath::FInterpTo(Yaw, TargetYaw, DeltaTime, RotationSpeed);
        CurrentCamera.AccumulatedPitch = FMath::FInterpTo(Pitch, TargetPitch, DeltaTime, RotationSpeed);
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
        SmTurrets[GetCameraIndex(CurrentCamera.CameraVehicle)]->SetRelativeRotation(FRotator(TurretAngle.Pitch, TurretAngle.Yaw, 0.f));
    }
}

FRotator AVehicleMaster::GetTurretAngle()
{
    if (!CurrentCamera.CameraVehicle) return CurrentAngle;

    const float Pitch = CurrentCamera.AccumulatedPitch;
    const float Yaw = CurrentCamera.AccumulatedYaw;

    if (SkeletalBaseVehicle)
    {
        CurrentAngle = FRotator(Pitch, Yaw, SkeletalBaseVehicle->GetSocketRotation(CurrentCamera.CameraVehicle->GetAttachParentSocketName()).Roll);
    }
    else if (BaseVehicle && !SmTurrets.IsEmpty())
    {
        CurrentAngle = FRotator(Pitch, Yaw, SmTurrets[GetCameraIndex(CurrentCamera.CameraVehicle)]->GetComponentRotation().Roll);
    }
	
    return CurrentAngle;
}

#pragma endregion