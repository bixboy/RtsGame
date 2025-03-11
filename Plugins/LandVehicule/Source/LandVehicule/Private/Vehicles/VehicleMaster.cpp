#include "Vehicles/VehicleMaster.h"
#include "CameraVehicle.h"
#include "CustomPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "VehiclesAnimInstance.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

// ------------------- Setup -------------------
#pragma region Setup

AVehicleMaster::AVehicleMaster()
{
    PrimaryActorTick.bCanEverTick = true;

    BaseVehicle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseVehicle"));
    RootComponent = BaseVehicle;

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    
    SpringArm->TargetArmLength = CameraDistance;
    SpringArm->TargetOffset = FVector(0, 0, 200.f);
    SpringArm->bEnableCameraRotationLag = true;
    SpringArm->CameraRotationLagSpeed = 10.0f;

    MainCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    MainCamera->SetupAttachment(SpringArm);
    MainCamera->bUsePawnControlRotation = true;

    bReplicates = true;
}

void AVehicleMaster::BeginPlay()
{
    Super::BeginPlay();

    if (SkeletalBaseVehicle)
        AnimInstance = Cast<UVehiclesAnimInstance>(SkeletalBaseVehicle->GetAnimInstance());

    InitializeCameras();
    
    SetReplicateMovement(true);
}

void AVehicleMaster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    
    if (auto* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AVehicleMaster::Input_OnMove);
        EnhancedInput->BindAction(MoveAction, ETriggerEvent::Completed, this, &AVehicleMaster::Input_OnMove);
        EnhancedInput->BindAction(InteractAction, ETriggerEvent::Started, this, &AVehicleMaster::Input_OnInteract);
        EnhancedInput->BindAction(ChangePlaceAction, ETriggerEvent::Started, this, &AVehicleMaster::Input_OnChangePlace);
    }
}

void AVehicleMaster::InitializeCameras()
{
    int32 CameraIndex = 1, TurretIndex = 0;
    for (UActorComponent* Component : GetComponentsByTag(UStaticMeshComponent::StaticClass(), "SmTurrets"))
    {
        if (UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(Component))
            SmTurrets.AddUnique(MeshComp);
    }
    
    for (UActorComponent* Component : GetComponentsByTag(UCameraComponent::StaticClass(), "PlaceCamera"))
    {
        if (UCameraComponent* Cam = Cast<UCameraComponent>(Component))
        {
            ACameraVehicle* SpawnedCamera = GetWorld()->SpawnActor<ACameraVehicle>(Cam->GetComponentLocation(), Cam->GetComponentRotation());
            if (SpawnedCamera)
            {
                FTurrets Turret;
                Turret.CameraVehicle = SpawnedCamera;
                Turrets.AddUnique(Turret);
                
                if (SkeletalBaseVehicle)
                {
                    FString SocketNameString = FString::Printf(TEXT("Place%d"), CameraIndex);
                    SpawnedCamera->AttachToComponent(SkeletalBaseVehicle, FAttachmentTransformRules::KeepWorldTransform, FName(*SocketNameString));
                }
                else if (BaseVehicle)
                {
                    FString SocketNameString = FString::Printf(TEXT("Place%d"), TurretIndex);
                    if (!SmTurrets.IsEmpty() && SmTurrets.IsValidIndex(TurretIndex))
                    {
                        SpawnedCamera->AttachToComponent(SmTurrets[TurretIndex], FAttachmentTransformRules::KeepWorldTransform);
                        TurretIndex++;
                    }
                }
                else
                {
                    SpawnedCamera->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
                }
                
                SpawnedCamera->SetActorRotation(Cam->GetComponentRotation());
                if (AnimInstance)
                    AnimInstance->TurretAngle.Add(SpawnedCamera->GetAttachParentSocketName(), FRotator::ZeroRotator);
                Cam->DestroyComponent();
                CameraIndex++;
            }
        }
    }
}

void AVehicleMaster::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME(AVehicleMaster, CurrentDriver);
    DOREPLIFETIME(AVehicleMaster, VehicleRoles);
    DOREPLIFETIME(AVehicleMaster, EngineOn);
    DOREPLIFETIME(AVehicleMaster, ForwardInput);
    DOREPLIFETIME(AVehicleMaster, TurnInput);
}

#pragma endregion

// ------------------- Possess -------------------
#pragma region Possess

void AVehicleMaster::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    if (APlayerController* PC = Cast<APlayerController>(NewController))
    {
        if (ACustomPlayerController* CustomPC = Cast<ACustomPlayerController>(PC))
        {
            SetOwner(PC);
            CustomPC->Client_AddMappingContext(NewMappingContext);
        }
    }
}

#pragma endregion

// ------------------- Interaction -------------------
#pragma region Interfaces

bool AVehicleMaster::Interact_Implementation(ACustomPlayerController* PlayerController)
{
    IVehiclesInteractions::Interact_Implementation(PlayerController);
    APawn* Player = PlayerController->GetPawn();
    if (PlacesNumber == CurrentPlace || GetRoleByPlayer(Player) == EVehiclePlaceType() || !HasAuthority())
        return false;
    
    if (!GetPlayerForRole(EVehiclePlaceType::Driver))
    {
        CurrentDriver = Player;
        AssignRole(Player, EVehiclePlaceType::Driver);
        PlayerController->Possess(this);
        PlayerController->SetViewTargetWithBlend(this, 0.1f, EViewTargetBlendFunction::VTBlend_Cubic, 0.f, false);
        CurrentPlace++;
        return true;
    }
    
    if (!bHaveTurret)
        return false;
    
    AssignRole(Player, EVehiclePlaceType::Gunner);
    SwitchToNextCamera(PlayerController);
    if (ACustomPlayerController* CustomPC = Cast<ACustomPlayerController>(PlayerController))
        CustomPC->Client_AddMappingContext(NewMappingContext);
    CurrentPlace++;
    return true;
}

void AVehicleMaster::OutOfVehicle_Implementation(ACustomPlayerController* PlayerController)
{
    IVehiclesInteractions::OutOfVehicle_Implementation(PlayerController);
    APawn* Player = PlayerController->GetPawn();
    if (!Player)
        return;
    
    if (this == Player)
    {
        if (CurrentDriver)
        {
            CurrentDriver->SetActorTransform(Player->GetActorTransform());
            PlayerController->Possess(CurrentDriver);
            PlayerController->Client_RemoveMappingContext(NewMappingContext);
            ReleaseRole(CurrentDriver);
            CurrentDriver = nullptr;
            CurrentPlace--;
        }
    }
    else
    {
        if (!CurrentCamera.CameraVehicle)
            return;
        PlayerController->SetViewTargetWithBlend(Player, 0.1f, EViewTargetBlendFunction::VTBlend_Cubic, 0.f, false);
        ReleaseRole(Player);
        CurrentCamera.CameraVehicle->SetIsUsed(false);
        PlayerController->Client_RemoveMappingContext(NewMappingContext);
        CurrentPlace--;
    }
}

void AVehicleMaster::Input_OnInteract()
{
    if (ACustomPlayerController* PC = Cast<ACustomPlayerController>(GetController()))
        PC->Server_OutOfVehicle(this);
}

void AVehicleMaster::Input_OnChangePlace()
{
    if (!bHaveTurret)
        return;
    if (ACustomPlayerController* PC = Cast<ACustomPlayerController>(GetController()))
        PC->Server_ChangeCamera(this);
}

void AVehicleMaster::ChangePlace_Implementation(ACustomPlayerController* Player)
{
    IVehiclesInteractions::ChangePlace_Implementation(Player);
    SwitchToNextCamera(Player);
}

void AVehicleMaster::UpdateTurretRotation_Implementation(FVector2D Rotation, FName TurretName, FTurrets CameraToMove)
{
    IVehiclesInteractions::UpdateTurretRotation_Implementation(Rotation, TurretName, CameraToMove);
    if (!bHaveTurret || Turrets.IsEmpty())
        return;
    
    ApplyTurretRotation(Rotation.X, Rotation.Y, TurretRotationSpeed, GetWorld()->GetTimeSeconds(), CameraToMove);
}

ACameraVehicle* AVehicleMaster::GetCurrentCameraVehicle_Implementation()
{
    return CurrentCamera.CameraVehicle;
}

#pragma endregion

// ------------------- Roles Management -------------------
#pragma region Roles Management

void AVehicleMaster::AssignRole(APawn* Player, EVehiclePlaceType RoleName) 
{
    if (!Player)
        return;
    for (const FVehicleRole& TempRole : VehicleRoles)
    {
        if (TempRole.RoleName == RoleName)
            return;
    }
    VehicleRoles.Add({ Player, RoleName });
}

void AVehicleMaster::ReleaseRole(APawn* Player)
{
    FVehicleRole RoleToRemove;
    for (const FVehicleRole& TempRole : VehicleRoles)
    {
        if (TempRole.Player == Player)
            RoleToRemove = TempRole;
    }
    VehicleRoles.Remove(RoleToRemove);
}

APawn* AVehicleMaster::GetPlayerForRole(EVehiclePlaceType RoleName) const
{
    for (const FVehicleRole& TempRole : VehicleRoles)
    {
        if (TempRole.RoleName == RoleName)
            return TempRole.Player;
    }
    return nullptr;
}

EVehiclePlaceType AVehicleMaster::GetRoleByPlayer(const APawn* Player) const
{
    for (const FVehicleRole& TempRole : VehicleRoles)
    {
        if (TempRole.Player == Player)
            return TempRole.RoleName;
    }
    return EVehiclePlaceType::None;
}

#pragma endregion

// ------------------- Camera Management -------------------
#pragma region Cameras

void AVehicleMaster::SwitchToCamera(APlayerController* PlayerController, const FTurrets& NewCamera)
{
    if (!bHaveTurret || Turrets.IsEmpty() || !PlayerController || !NewCamera.CameraVehicle || NewCamera.CameraVehicle->GetIsUsed())
        return;
    
    if (!HasAuthority())
    {
        Server_SwitchToCamera(PlayerController, NewCamera);
        return;
    }
    
    if (FTurrets* CurrentTurret = Turrets.FindByPredicate([PlayerController](const FTurrets& T)
    {
        return T.CameraVehicle && T.CameraVehicle->GetIsUsed() &&
               T.CameraVehicle->GetCameraController() == PlayerController;
    }))
    {
        CurrentTurret->CameraVehicle->SetIsUsed(false);
        CurrentTurret->CameraVehicle->SetController(nullptr);
        PlayersInVehicle.Remove(PlayerController);
    }
    
    NewCamera.CameraVehicle->SetIsUsed(true);
    NewCamera.CameraVehicle->SetController(PlayerController);
    
    if (FTurrets* OldTurret = Turrets.FindByPredicate([this](const FTurrets& T)
    {
        return T.CameraVehicle == CurrentCamera.CameraVehicle;
    }))
    {
        OldTurret->AccumulatedPitch = CurrentCamera.AccumulatedPitch;
        OldTurret->AccumulatedYaw = CurrentCamera.AccumulatedYaw;
    }
    
    Client_SwitchToCamera(PlayerController, NewCamera);
    PlayersInVehicle.FindOrAdd(PlayerController, CurrentCamera.CameraVehicle);
    
    if (PlayerController->GetPawn() == CurrentDriver)
    {
        if (ACustomPlayerController* CustomPC = Cast<ACustomPlayerController>(PlayerController))
            CustomPC->Client_AddMappingContext(NewMappingContext);
        
        ReleaseRole(CurrentDriver);
        AssignRole(CurrentDriver, EVehiclePlaceType::Gunner);
        CurrentDriver = nullptr;
    }
    
    PlayerController->SetViewTargetWithBlend(NewCamera.CameraVehicle, 0.1f, EViewTargetBlendFunction::VTBlend_Cubic, 0.f, false);
}

void AVehicleMaster::Server_SwitchToCamera_Implementation(APlayerController* PlayerController, const FTurrets NewCamera)
{
    SwitchToCamera(PlayerController, NewCamera);
}

void AVehicleMaster::Client_SwitchToCamera_Implementation(APlayerController* PlayerController, const FTurrets NewCamera)
{
    if (!NewCamera.CameraVehicle)
        return;
    
    CurrentCamera = NewCamera;
}

void AVehicleMaster::SwitchToNextCamera(APlayerController* PlayerController)
{
    if (!bHaveTurret || !PlayerController || Turrets.IsEmpty())
        return;
    
    APawn* Player = PlayerController->GetPawn();
    if (!Player)
        return;
    
    if (this == Player)
    {
        PlayerController->Possess(CurrentDriver);
        PlayerController = Cast<APlayerController>(CurrentDriver->GetController());
    }
    
    for (int32 i = 0; i < Turrets.Num(); i++)
    {
        FTurrets Turret = Turrets[i];
        if (Turret.CameraVehicle && Turret.CameraVehicle->GetIsUsed() && Turret.CameraVehicle->GetCameraController() == PlayerController)
        {
            Turret.CameraVehicle->SetIsUsed(false);
            Turret.CameraVehicle->SetController(nullptr);
    
            if (i == Turrets.Num() - 1 || !GetAvailableCamera(i).CameraVehicle)
            {
                if (!CurrentDriver && MainCamera)
                {
                    SwitchToMainCam(PlayerController);
                    return;
                }
            }
            
            FTurrets NextAvailableCamera = GetAvailableCamera(i);
            if (NextAvailableCamera.CameraVehicle)
            {
                SwitchToCamera(PlayerController, NextAvailableCamera);
                return;
            }
    
            Turret.CameraVehicle->SetIsUsed(true);
            Turret.CameraVehicle->SetController(PlayerController);
            return;
        }
    }
    
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
        PlayerController->SetViewTargetWithBlend(this, 0.1f, EViewTargetBlendFunction::VTBlend_Cubic, 0.f, false);
    }
}

ACameraVehicle* AVehicleMaster::GetAttachedCamera(FName ParentName)
{
    if (Turrets.IsEmpty())
        return nullptr;
    for (const FTurrets& Turret : Turrets)
    {
        if (Turret.CameraVehicle->GetAttachParentSocketName() == ParentName)
            return Turret.CameraVehicle;
    }
    return nullptr;
}

int AVehicleMaster::GetCameraIndex(const ACameraVehicle* CameraVehicle)
{
    for (int32 i = 0; i < Turrets.Num(); i++)
    {
        if (Turrets[i].CameraVehicle == CameraVehicle)
            return i;
    }
    return 0;
}

FTurrets AVehicleMaster::GetAvailableCamera(int startIndex)
{
    for (int32 j = 1; j < Turrets.Num(); j++)
    {
        int32 NextIndex = (startIndex + j) % Turrets.Num();
        FTurrets NextCamera = Turrets[NextIndex];
        if (NextCamera.CameraVehicle && !NextCamera.CameraVehicle->GetIsUsed())
            return NextCamera;
    }
    return FTurrets();
}

FTurrets AVehicleMaster::GetCurrentCamera()
{
    return CurrentCamera;
}

#pragma endregion

// ------------------- Vehicle Controls -------------------
#pragma region Vehicle Control

void AVehicleMaster::TurnEngineOn(bool OnOff)
{
    EngineOn = OnOff;
    PlaySound(SoundEngineOn);
}

void AVehicleMaster::Input_OnMove(const FInputActionValue& InputActionValue)
{
    FVector2D MovementVector = InputActionValue.Get<FVector2D>();
    Server_OnMove(MovementVector.X, MovementVector.Y);
}

void AVehicleMaster::Server_OnMove_Implementation(float InForward, float InTurn)
{
    ForwardInput = InForward;
    TurnInput = InTurn;
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

// ------------------- Turret Controls -------------------
#pragma region Turret Control

void AVehicleMaster::ApplyTurretRotation(float DeltaYaw, float DeltaPitch, float RotationSpeed, float DeltaTime, FTurrets CameraToMove)
{
    if (CameraToMove.CameraVehicle)
    {
        int Index = -1;
        for (int i = 0; i < Turrets.Num(); ++i)
        {
            if (Turrets[i].CameraVehicle == CameraToMove.CameraVehicle)
            {
                Index = i;
                break;
            }
        }
        
        if (Index != -1)
        { 
            float Pitch = CameraToMove.AccumulatedPitch;
            float Yaw = CameraToMove.AccumulatedYaw;
        
            float TargetYaw = FMath::Clamp(Yaw + DeltaYaw, -120.f, 120.f);
            float TargetPitch = FMath::Clamp(Pitch + DeltaPitch, -20.f, 20.f);
            Turrets[Index].AccumulatedYaw = FMath::FInterpTo(Yaw, TargetYaw, DeltaTime, RotationSpeed);
            Turrets[Index].AccumulatedPitch = FMath::FInterpTo(Pitch, TargetPitch, DeltaTime, RotationSpeed);

            Multicast_SetTurretRotation(Turrets[Index].CameraVehicle, GetTurretAngle());   
        }
    }
}

void AVehicleMaster::Multicast_SetTurretRotation_Implementation(ACameraVehicle* Camera, FRotator TurretAngle)
{
    if (SkeletalBaseVehicle && AnimInstance)
    {
        FName TurretName = Camera->GetAttachParentSocketName();
        AnimInstance->UpdateTurretRotation(TurretAngle, TurretName);   
    }
    else if (BaseVehicle)
    {
        SmTurrets[GetCameraIndex(Camera)]->SetRelativeRotation(FRotator(TurretAngle.Pitch, TurretAngle.Yaw, 0.f));
    }
}

FRotator AVehicleMaster::GetTurretAngle()
{
    if (!CurrentCamera.CameraVehicle)
        return CurrentAngle;
    
    float Pitch = CurrentCamera.AccumulatedPitch;
    float Yaw = CurrentCamera.AccumulatedYaw;
    
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
