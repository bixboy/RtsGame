#include "Vehicles/VehicleMaster.h"
#include "CameraVehicle.h"
#include "CustomPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "VehiclesAnimInstance.h"
#include "Components/AudioComponent.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "Vehicles/VehiclePlayerMesh.h"


// ------------------- Setup -------------------
#pragma region Setup

AVehicleMaster::AVehicleMaster()
{
    PrimaryActorTick.bCanEverTick = true;

    BaseVehicle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseVehicle"));
    RootComponent = BaseVehicle;

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->bUsePawnControlRotation = false;
    
    SpringArm->TargetArmLength = CameraDistance;
    SpringArm->TargetOffset = FVector(0, 0, 200.f);
    SpringArm->bEnableCameraRotationLag = true;
    SpringArm->CameraRotationLagSpeed = 10.0f;

    MainCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    MainCamera->SetupAttachment(SpringArm);
    MainCamera->bUsePawnControlRotation = true;

    bReplicates = true;

    // Audio Setup
    USceneComponent* AudioFill = CreateDefaultSubobject<USceneComponent>(TEXT("AudioFill"));
    AudioFill->SetupAttachment(BaseVehicle);

    StartEngineAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("StartEngineAudio"));
    StartEngineAudio->SetupAttachment(AudioFill);
    
    EngineAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("EngineAudio"));
    EngineAudio->SetupAttachment(AudioFill);

    MovementAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("MovementAudio"));
    MovementAudio->SetupAttachment(AudioFill);
    
}

void AVehicleMaster::BeginPlay()
{
    Super::BeginPlay();

    SetReplicateMovement(true);

    if (SkeletalBaseVehicle)
        AnimInstance = Cast<UVehiclesAnimInstance>(SkeletalBaseVehicle->GetAnimInstance());

    if (SpringArm)
        SpringArm->bUsePawnControlRotation = false;

    InitializeCameras();

    
    GetComponents<UVehiclePlayerMesh>(VehiclePlayersMesh);
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
        
        EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &AVehicleMaster::Input_OnUpdateCameraRotation);

        EnhancedInput->BindAction(SwitchEngine, ETriggerEvent::Started, this, &AVehicleMaster::Input_SwitchEngine);
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
                Turrets.AddUnique(SpawnedCamera);
                
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
    DOREPLIFETIME(AVehicleMaster, bEngineOn);
    DOREPLIFETIME(AVehicleMaster, ForwardInput);
    DOREPLIFETIME(AVehicleMaster, TurnInput);
}

void AVehicleMaster::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    FRotator VehicleRotation;
    if (BaseVehicle)
    {
        VehicleRotation = BaseVehicle->GetComponentRotation();
    }
    else if (SkeletalBaseVehicle)
    {
        VehicleRotation = SkeletalBaseVehicle->GetComponentRotation();
    }
    
    FRotator TargetSpringArmRotation = VehicleRotation + CameraRotationOffset;
    if (SpringArm)
    {
        FRotator NewRotation = FMath::RInterpTo(SpringArm->GetComponentRotation(), TargetSpringArmRotation, DeltaSeconds, 8.f);
        SpringArm->SetWorldRotation(NewRotation);
    }

    // Sounds
    if (bEngineOn && HasAuthority())
    {
        // === Movement ===
        if (ForwardInput > 0.1f)
        {
            Multicast_StopSound(EngineAudio);
            
            if (!GetSoundIsPlaying(SoundDriveLoop, MovementAudio))
            {
                Multicast_PlaySound(SoundDriveLoop, MovementAudio);
            }
        }

        // === Idle ===
        else if (ForwardInput == 0.f)
        {
            Multicast_StopSound(MovementAudio);

            if (!GetSoundIsPlaying(SoundIdleLoop, EngineAudio))
            {
                Multicast_PlaySound(SoundIdleLoop, EngineAudio);
            }
        }
    }
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

// Interaction
void AVehicleMaster::Input_OnInteract()
{
    if (ACustomPlayerController* PC = Cast<ACustomPlayerController>(GetController()))
    {
        PC->Server_OutOfVehicle(this);
    }
}

bool AVehicleMaster::Interact_Implementation(ACustomPlayerController* PlayerController)
{
    IVehiclesInteractions::Interact_Implementation(PlayerController);
    
    if (!HasAuthority()) return false;
    
    APawn* Player = PlayerController->GetPawn();
    if (PlacesNumber == CurrentPlace || GetRoleByPlayer(Player) == EVehiclePlaceType())
        return false;

    
    // --------- Cas 1 : Affectation de la place conducteur (Driver Place) ---------
    if (!GetPlayerByRole(EVehiclePlaceType::Driver))
    {
        CurrentDriver = Player;
        AssignRole(Player, EVehiclePlaceType::Driver);

        if (!VehiclePlayersMesh.IsEmpty())
        {
            for (UVehiclePlayerMesh* VehiclePlayerMesh : VehiclePlayersMesh)
            {
                if (VehiclePlayerMesh->PlaceNumber == 1)
                {
                    ACharacter* PlayerCharacter = PlayerController->GetCharacter();
                    VehiclePlayerMesh->SetupPlayerMesh(PlayerCharacter->GetMesh()->GetSkeletalMeshAsset());
                    
                    PlayersMeshAssigned.Add(PlayerController, VehiclePlayerMesh);
                    
                    break;
                }
            }
        }
        
        PlayerController->GetCharacter()->SetActorHiddenInGame(true);
        PlayerController->Possess(this);
        PlayerController->SetViewTargetWithBlend(this, 0.1f, EViewTargetBlendFunction::VTBlend_Cubic, 0.f, false);
        CurrentPlace++;
        
        return true;
    }
    
    if (!bHaveTurret)
        return false;

    // --------- Cas 2 : Affectation de la place tireur (Gunner Place) ---------
    {
        AssignRole(Player, EVehiclePlaceType::Gunner);
        SwitchToNextCamera(PlayerController);
        
        PlayerController->Client_AddMappingContext(NewMappingContext);
        PlayerController->GetCharacter()->SetActorHiddenInGame(true);
    
        CurrentPlace++;
        ShowPlayerMesh(PlayerController);
        
        return true;   
    }
}

void AVehicleMaster::OutOfVehicle_Implementation(ACustomPlayerController* PlayerController)
{
 IVehiclesInteractions::OutOfVehicle_Implementation(PlayerController);

    APawn* PlayerPawn = PlayerController->GetPawn();
    if (!PlayerPawn)
    {
        return;
    }
    
    FVector ExitLocation = FindClosestExitLocation(PlayerController);

    // ===== Cas 1 : Le véhicule (this) est le Pawn possédé par le joueur =====
    if (this == PlayerPawn)
    {
        if (CurrentDriver)
        {
            PlayerController->Possess(CurrentDriver);
            PlayerController->Client_RemoveMappingContext(NewMappingContext);

            if (PlayerController->GetCharacter())
            {
                PlayerController->GetCharacter()->SetActorLocation(ExitLocation);
                PlayerController->GetCharacter()->SetActorHiddenInGame(false);
            }

            ReleaseRole(CurrentDriver);
            CurrentDriver = nullptr;
            CurrentPlace--;

            HidePlayerMesh(PlayerController, 1);
        }
    }
    // ===== Cas 2 : Le véhicule n'est pas le Pawn possédé directement =====
    else
    {
        ACameraVehicle* Camera = AssignedCameras.FindRef(PlayerController);
        if (!Camera)
        {
            return;
        }

        if (PlayerController->GetCharacter())
        {
            PlayerController->GetCharacter()->SetActorLocation(ExitLocation);
            PlayerController->GetCharacter()->SetActorHiddenInGame(false);
        }

        PlayerController->SetViewTargetWithBlend(PlayerPawn, 0.1f, EViewTargetBlendFunction::VTBlend_Cubic, 0.f, false);
        PlayerController->Client_RemoveMappingContext(NewMappingContext);

        AssignedCameras.Remove(PlayerController);
        Camera->SetIsUsed(false);
        Camera->SetController(nullptr);

        ReleaseRole(PlayerPawn);
        CurrentPlace--;

        HidePlayerMesh(PlayerController);
    }
}


// Change Place
void AVehicleMaster::Input_OnChangePlace()
{
    if (!bHaveTurret) return;
    
    if (ACustomPlayerController* PC = Cast<ACustomPlayerController>(GetController()))
    {
        PC->Server_ChangeCamera(this);
    }
}

ACameraVehicle* AVehicleMaster::ChangePlace_Implementation(ACustomPlayerController* Player)
{
    IVehiclesInteractions::ChangePlace_Implementation(Player);
    
    return SwitchToNextCamera(Player);
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

APawn* AVehicleMaster::GetPlayerByRole(EVehiclePlaceType RoleName) const
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

void AVehicleMaster::Input_OnUpdateCameraRotation(const FInputActionValue& InputActionValue)
{
    FVector2D InputVector = InputActionValue.Get<FVector2D>();

    CameraRotationOffset.Pitch += (InputVector.Y * Sensitivity);
    CameraRotationOffset.Yaw   += (InputVector.X * Sensitivity);

    CameraRotationOffset.Pitch = FMath::Clamp(CameraRotationOffset.Pitch, -80.f, 80.f);
}


void AVehicleMaster::SwitchToCamera(APlayerController* PlayerController, ACameraVehicle* NewCamera)
{
    if (!bHaveTurret || Turrets.IsEmpty() || !PlayerController || !NewCamera || NewCamera->GetIsUsed())
        return;
    
    if (!HasAuthority())
    {
        Server_SwitchToCamera(PlayerController, NewCamera);
        return;
    }
    
    if (ACameraVehicle** FoundTurretPtr = Turrets.FindByPredicate([PlayerController](const ACameraVehicle* T)
    {
        return T && T->GetIsUsed() && T->GetCameraController() == PlayerController;
    }))
    {
        ACameraVehicle* CurrentTurret = *FoundTurretPtr;
        
        CurrentTurret->SetIsUsed(false);
        CurrentTurret->SetController(nullptr);
        AssignedCameras.Remove(PlayerController);
    }
    
    NewCamera->Turret.CameraVehicle->SetIsUsed(true);
    NewCamera->Turret.CameraVehicle->SetController(PlayerController);
    AssignedCameras.FindOrAdd(PlayerController, NewCamera);

    int Index = Turrets.Find(NewCamera) + 1;
    if (Index != 1)
    {
        ShowPlayerMesh(PlayerController, Index);
    }
    
    if (PlayerController->GetPawn() == CurrentDriver)
    {
        if (ACustomPlayerController* CustomPC = Cast<ACustomPlayerController>(PlayerController))
            CustomPC->Client_AddMappingContext(NewMappingContext);
        
        ReleaseRole(CurrentDriver);
        AssignRole(CurrentDriver, EVehiclePlaceType::Gunner);
        CurrentDriver = nullptr;
    }
    
    PlayerController->SetViewTargetWithBlend(NewCamera, 0.1f, EViewTargetBlendFunction::VTBlend_Cubic, 0.f, false);
}

void AVehicleMaster::Server_SwitchToCamera_Implementation(APlayerController* PlayerController, ACameraVehicle* NewCamera)
{
    SwitchToCamera(PlayerController, NewCamera);
}

// -------- Next Camera --------
ACameraVehicle* AVehicleMaster::SwitchToNextCamera(APlayerController* PlayerController)
{
    if (!bHaveTurret || !PlayerController || Turrets.IsEmpty())
        return nullptr;
    
    APawn* Player = PlayerController->GetPawn();
    if (!Player)
    {
        return nullptr;   
    }
    
    if (this == Player)
    {
        PlayerController->Possess(CurrentDriver);
        PlayerController = Cast<APlayerController>(CurrentDriver->GetController());
    }
    
    for (int32 i = 0; i < Turrets.Num(); i++)
    {
        ACameraVehicle* Turret = Turrets[i];
        if (Turret && Turret->GetIsUsed() && Turret->GetCameraController() == PlayerController)
        {
            Turret->SetIsUsed(false);
            Turret->SetController(nullptr);
    
            if (i == Turrets.Num() - 1 || !GetAvailableCamera(i))
            {
                if (!CurrentDriver && MainCamera)
                {
                    SwitchToMainCam(PlayerController);
                    return nullptr;
                }
            }
            
            ACameraVehicle* NextAvailableCamera = GetAvailableCamera(i);
            if (NextAvailableCamera)
            {
                SwitchToCamera(PlayerController, NextAvailableCamera);
                
                return NextAvailableCamera;
            }
    
            Turret->SetIsUsed(true);
            Turret->SetController(PlayerController);
            return Turret;
        }
    }
    
    for (ACameraVehicle* Turret : Turrets)
    {
        if (Turret && !Turret->GetIsUsed())
        {
            SwitchToCamera(PlayerController, Turret);
            
            return Turret;
        }
    }
    
    UE_LOG(LogTemp, Warning, TEXT("No available camera for player: %s"), *PlayerController->GetName());
    return nullptr;
}

void AVehicleMaster::SwitchToMainCam(APlayerController* PlayerController)
{
    if (!CurrentDriver)
    {
        CurrentDriver = PlayerController->GetCharacter();
        AssignRole(PlayerController->GetCharacter(), EVehiclePlaceType::Driver);
        
        ShowPlayerMesh(PlayerController, 1);
        
        PlayerController->Possess(this);
        PlayerController->SetViewTargetWithBlend(this, 0.1f, EViewTargetBlendFunction::VTBlend_Cubic, 0.f, false);
    }
}


// -------- Utilities --------
ACameraVehicle* AVehicleMaster::GetAttachedCamera(FName ParentName)
{
    if (Turrets.IsEmpty())
        return nullptr;
    for (ACameraVehicle* Turret : Turrets)
    {
        if (Turret->GetAttachParentSocketName() == ParentName)
            return Turret;
    }
    return nullptr;
}

ACameraVehicle* AVehicleMaster::GetAvailableCamera(int startIndex)
{
    for (int32 j = 1; j < Turrets.Num(); j++)
    {
        int32 NextIndex = (startIndex + j) % Turrets.Num();
        ACameraVehicle* NextCamera = Turrets[NextIndex];
        if (NextCamera && !NextCamera->GetIsUsed())
            return NextCamera;
    }
    return nullptr;
}

#pragma endregion


// ------------------- Player Mesh -------------------
#pragma region Player Mesh

int AVehicleMaster::ShowPlayerMesh(APlayerController* PlayerController, int PlaceNum)
{
     int32 AssignedIndex = -1;

    if (PlayersMeshAssigned.Contains(PlayerController))
    {
        UVehiclePlayerMesh* PreviouslyAssigned = PlayersMeshAssigned[PlayerController];
        int32 PrevIndex = PreviouslyAssigned->PlaceNumber;

        HidePlayerMesh(PlayerController, PrevIndex);
    }

    if (PlaceNum == -1)
    {
        for (int32 Index = 0; Index < VehiclePlayersMesh.Num(); Index++)
        {
            UVehiclePlayerMesh* VehiclePlayerMesh = VehiclePlayersMesh[Index];
            if (VehiclePlayerMesh && VehiclePlayerMesh->PlaceNumber != 1 && !VehiclePlayerMesh->bIsUsed)
            {
                VehiclePlayerMesh->SetupPlayerMesh(PlayerController->GetCharacter()->GetMesh()->GetSkeletalMeshAsset());
                VehiclePlayerMesh->bIsUsed = true;
                
                PlayersMeshAssigned.Add(PlayerController, VehiclePlayerMesh);
                
                AssignedIndex = Index;
                return AssignedIndex;
            }
        }
    }
    else if (PlaceNum >= 0)
    {
        for (int32 Index = 0; Index < VehiclePlayersMesh.Num(); Index++)
        {
            UVehiclePlayerMesh* VehiclePlayerMesh = VehiclePlayersMesh[Index];
            if (VehiclePlayerMesh && VehiclePlayerMesh->PlaceNumber == PlaceNum && !VehiclePlayerMesh->bIsUsed)
            {
                VehiclePlayerMesh->SetupPlayerMesh(PlayerController->GetCharacter()->GetMesh()->GetSkeletalMeshAsset());
                VehiclePlayerMesh->bIsUsed = true;
                
                PlayersMeshAssigned.Add(PlayerController, VehiclePlayerMesh);
                
                AssignedIndex = Index;
                return AssignedIndex;
            }
        }
        UE_LOG(LogTemp, Warning, TEXT("ShowPlayerMesh : Aucun slot libre trouvé pour la place %d."), PlaceNum);
    }

    return AssignedIndex;
}

void AVehicleMaster::HidePlayerMesh(APlayerController* PlayerController, int MeshNumber)
{
    if (!VehiclePlayersMesh.IsEmpty())
    {
        if (MeshNumber > 0)
        {
            for (UVehiclePlayerMesh* VehiclePlayerMesh : VehiclePlayersMesh)
            {
                if (VehiclePlayerMesh->bIsUsed && VehiclePlayerMesh->PlaceNumber == MeshNumber)
                {
                    VehiclePlayerMesh->HidePlayerMesh();
                    PlayersMeshAssigned.Remove(PlayerController);
                
                    break;
                }
            }   
        }
        else
        {
            UVehiclePlayerMesh* PlayerMesh = PlayersMeshAssigned.FindRef(PlayerController);
            if (PlayerMesh && PlayerMesh->bIsUsed)
            {
                PlayerMesh->HidePlayerMesh();
                PlayersMeshAssigned.Remove(PlayerController);
            }
        }
    }
}

UVehiclePlayerMesh* AVehicleMaster::GetPlayerMesh(APlayerController* PlayerController)
{
    if (PlayersMeshAssigned.IsEmpty()) return nullptr;

    return PlayersMeshAssigned[PlayerController];
}

FVector AVehicleMaster::FindClosestExitLocation(APlayerController* PlayerController)
{
   UVehiclePlayerMesh* PlayerMesh = GetPlayerMesh(PlayerController);
    if (!PlayerMesh)
    {
        return GetActorLocation();
    }
    
    FVector BaseLocation = PlayerMesh->GetComponentLocation();
    UWorld* World = GetWorld();
    
    const float MinOffset = 50.f;       // Distance minimale à tester
    const float MaxOffset = 300.f;      // Distance maximale à tester
    const float StepSize = 25.f;        // Incrément de distance entre chaque test
    const float SphereRadius = 20.f;    // Rayon du trace sphérique

    float BestDistance = FLT_MAX;
    FVector BestLocation = BaseLocation;
    FVector BestDir = FVector::ZeroVector;

    TArray<FVector> Directions;
    Directions.Add(GetActorForwardVector());
    Directions.Add(-GetActorForwardVector());
    Directions.Add(GetActorRightVector());
    Directions.Add(-GetActorRightVector());
    Directions.Add((GetActorForwardVector() + GetActorRightVector()).GetSafeNormal());
    Directions.Add((GetActorForwardVector() - GetActorRightVector()).GetSafeNormal());
    Directions.Add((-GetActorForwardVector() + GetActorRightVector()).GetSafeNormal());
    Directions.Add((-GetActorForwardVector() - GetActorRightVector()).GetSafeNormal());

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredComponent(PlayerMesh);

    for (const FVector& Dir : Directions)
    {
        for (float Offset = MinOffset; Offset <= MaxOffset; Offset += StepSize)
        {
            FVector CandidateLocation = BaseLocation + Dir * Offset;
            
            FHitResult HitResult;
            bool bHit = World->SweepSingleByChannel(
                HitResult,
                CandidateLocation,
                CandidateLocation,
                FQuat::Identity,
                ECC_WorldStatic,
                FCollisionShape::MakeSphere(SphereRadius),
                QueryParams
            );
            
            // Affichage debug
            DrawDebugLine(World, BaseLocation, CandidateLocation, bHit ? FColor::Red : FColor::Green, false, 2.f, 0, 2.f);
            DrawDebugSphere(World, bHit ? HitResult.Location : CandidateLocation, SphereRadius, 12, bHit ? FColor::Red : FColor::Green, false, 2.f);
            
            if (!bHit)
            {
                if (Offset < BestDistance)
                {
                    BestDistance = Offset;
                    BestLocation = CandidateLocation;
                    BestDir = Dir;
                }
                break;
            }
        }
    }
    
    if (!BestDir.IsNearlyZero())
    {
        BestLocation += BestDir * OutOfVehicleOffset;
    }
    
    return BestLocation;
}

#pragma endregion


// ------------------- Vehicle Controls -------------------
#pragma region Vehicle Control

void AVehicleMaster::Input_SwitchEngine()
{
    Server_SwitchEngine(!bEngineOn);
}

void AVehicleMaster::Server_SwitchEngine_Implementation(bool OnOff)
{
    if (HasAuthority())
    {
        if (OnOff)
        {
            Multicast_PlaySound(SoundEngineOn, StartEngineAudio);
        }
        else
        {
            Multicast_StopSound(MovementAudio);
            Multicast_StopSound(EngineAudio);
            
            Multicast_PlaySound(SoundEngineOff, StartEngineAudio);
            bEngineOn = OnOff;
            
            return;
        }
        
        FTimerDelegate TimerDelegate;
        TimerDelegate.BindLambda([this, OnOff]()
        {
            bEngineOn = OnOff;
            
            if (bEngineOn)
                Multicast_PlaySound(SoundIdleLoop, EngineAudio);
        });
        
        FTimerHandle DelayTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(DelayTimerHandle, TimerDelegate, 0.5f, false); 
    }
}

void AVehicleMaster::Input_OnMove(const FInputActionValue& InputActionValue)
{
    FVector2D MovementVector = InputActionValue.Get<FVector2D>();
    Server_OnMove(MovementVector.X, MovementVector.Y);

    OnVehicleMove.Broadcast(MovementVector.X, MovementVector.Y);
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


// ------------------- Sounds -------------------
#pragma region Sounds

void AVehicleMaster::Multicast_PlaySound_Implementation(USoundBase* Sound, UAudioComponent* AudioComp)
{
    if (!Sound || !AudioComp) return;
    
    AudioComp->Stop();
    AudioComp->SetSound(Sound);
    AudioComp->Play();
}

void AVehicleMaster::Multicast_StopSound_Implementation(UAudioComponent* AudioComp)
{
    if (!AudioComp) return;

    AudioComp->Stop();
}

bool AVehicleMaster::GetSoundIsPlaying(USoundBase* Sound, UAudioComponent* AudioComp)
{
    if (!Sound || !AudioComp) return false;

    return AudioComp->GetSound() == Sound && AudioComp->IsPlaying();
}

#pragma endregion


// ------------------- Turret Controls -------------------
#pragma region Turret Control

void AVehicleMaster::ApplyTurretRotation(float DeltaYaw, float DeltaPitch, float RotationSpeed, float DeltaTime, ACameraVehicle* CameraToMove)
{
    if (CameraToMove)
    {
        float Pitch = CameraToMove->Turret.AccumulatedPitch;
        float Yaw = CameraToMove->Turret.AccumulatedYaw;
    
        float TargetYaw = FMath::Clamp(Yaw + DeltaYaw, -120.f, 120.f);
        float TargetPitch = FMath::Clamp(Pitch + DeltaPitch, -20.f, 20.f);
        
        CameraToMove->Turret.AccumulatedYaw = FMath::FInterpTo(Yaw, TargetYaw, DeltaTime, RotationSpeed);
        CameraToMove->Turret.AccumulatedPitch = FMath::FInterpTo(Pitch, TargetPitch, DeltaTime, RotationSpeed);

        Multicast_SetTurretRotation(CameraToMove, Turrets.Find(CameraToMove),  GetTurretAngle(CameraToMove));   
    }
}

void AVehicleMaster::Multicast_SetTurretRotation_Implementation(ACameraVehicle* Camera, int IndexOfCamera, FRotator TurretAngle)
{
    if (SkeletalBaseVehicle && AnimInstance)
    {
        FName TurretName = Camera->GetAttachParentSocketName();
        AnimInstance->UpdateTurretRotation(TurretAngle, TurretName);   
    }
    else if (BaseVehicle)
    {
        SmTurrets[IndexOfCamera]->SetRelativeRotation(FRotator(TurretAngle.Pitch, TurretAngle.Yaw, 0.f));
    }
}

void AVehicleMaster::OnTurretRotate(FVector2D NewRotation, ACameraVehicle* CameraToRotate)
{
    if (!bHaveTurret || Turrets.IsEmpty())
        return;
    
   ApplyTurretRotation(NewRotation.X, NewRotation.Y, TurretRotationSpeed, GetWorld()->GetTimeSeconds(), CameraToRotate);
}


FRotator AVehicleMaster::GetTurretAngle(ACameraVehicle* Camera)
{
    if (!Camera)
        return CurrentAngle;
    
    float Pitch = Camera->Turret.AccumulatedPitch;
    float Yaw = Camera->Turret.AccumulatedYaw;
    
    if (SkeletalBaseVehicle)
    {
        CurrentAngle = FRotator(Pitch, Yaw, SkeletalBaseVehicle->GetSocketRotation(Camera->GetAttachParentSocketName()).Roll);
    }
    else if (BaseVehicle && !SmTurrets.IsEmpty())
    {
        CurrentAngle = FRotator(Pitch, Yaw, SmTurrets[Turrets.Find(Camera)]->GetComponentRotation().Roll);
    }
    
    return CurrentAngle;
}

#pragma endregion
