#include "Vehicles/VehicleMaster.h"
#include "CameraVehicle.h"
#include "CustomPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "VehiclesAnimInstance.h"
<<<<<<< Updated upstream
#include "Component/VehicleCamera.h"
=======
#include "Component/SeatComponent.h"
#include "Component/TurretCamera.h"
>>>>>>> Stashed changes
#include "Components/AudioComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
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
    
    MainCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    MainCamera->SetupAttachment(SpringArm);

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

<<<<<<< Updated upstream
    if (HasAuthority())
    {
        InitializeCameras();
        GetComponents<UVehiclePlayerMesh>(VehiclePlayersMesh);   
    }
=======
    SetupSeats();
    InitializeTurrets();

>>>>>>> Stashed changes
}

void AVehicleMaster::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    if (SpringArm && MainCamera)
    {
        SpringArm->bUsePawnControlRotation = false;
        MainCamera->bUsePawnControlRotation = false;
        
        SpringArm->TargetArmLength = CameraDistance;
        SpringArm->TargetOffset = FVector(0, 0, 200.f);
        
        SpringArm->bEnableCameraRotationLag = true;
    }

    if (!bCanRotateCamera)
    {
        SpringArm->bEnableCameraLag = true;
    }
}

<<<<<<< Updated upstream
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

    TArray<UVehicleCamera*> Components;
    GetComponents<UVehicleCamera>(Components);
    
    for (UVehicleCamera* Component : Components)
    {
        ACameraVehicle* SpawnedCamera = GetWorld()->SpawnActor<ACameraVehicle>(Component->GetComponentLocation(), Component->GetComponentRotation());
        if (SpawnedCamera)
        {
            SpawnedCamera->SetIsTurret(Component->bIsTurrets);
            SpawnedCamera->SetSwitchToOtherTypeCam(Component->bSwitchToOtherTypeCam);
            
            AllCameras.AddUnique(SpawnedCamera);

            if (SpawnedCamera->GetIsTurret())
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
            }
            else
            {
                SpawnedCamera->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
            }
            
            if (AnimInstance)
            {
                AnimInstance->TurretAngle.Add(SpawnedCamera->GetAttachParentSocketName(), FRotator::ZeroRotator);   
            }

            SpawnedCamera->SetActorRotation(Component->GetComponentRotation());
            
            Component->DestroyComponent();
            CameraIndex++;
        }
    }
}

=======
>>>>>>> Stashed changes
void AVehicleMaster::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME(AVehicleMaster, CurrentDriver);
    DOREPLIFETIME(AVehicleMaster, VehicleRoles);
    DOREPLIFETIME(AVehicleMaster, bEngineOn);
    DOREPLIFETIME(AVehicleMaster, ForwardInput);
    DOREPLIFETIME(AVehicleMaster, TurnInput);
    DOREPLIFETIME(AVehicleMaster, bCanRotateCamera);
}

void AVehicleMaster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    
    if (auto* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInput->BindAction(SwitchViewModeAction, ETriggerEvent::Started,   this, &AVehicleMaster::Input_SwitchViewMode);
        EnhancedInput->BindAction(SwitchEngine,         ETriggerEvent::Started,   this, &AVehicleMaster::Input_SwitchEngine);
        EnhancedInput->BindAction(ExitAction,           ETriggerEvent::Started,   this, &AVehicleMaster::Input_OnExit);
        EnhancedInput->BindAction(MoveAction,           ETriggerEvent::Triggered, this, &AVehicleMaster::Input_OnMove);
        EnhancedInput->BindAction(MoveAction,           ETriggerEvent::Completed, this, &AVehicleMaster::Input_OnMove);
        EnhancedInput->BindAction(LookAction,           ETriggerEvent::Triggered, this, &AVehicleMaster::Input_OnUpdateCameraRotation);
    }
}


void AVehicleMaster::SetupSeats()
{
    TArray<USeatComponent*> SeatComps;
    GetComponents(SeatComps);

    SeatComps.Sort([](const USeatComponent& A, const USeatComponent& B) {
        return A.SeatIndex < B.SeatIndex;
    });

    Seats.Empty();
    for (USeatComponent* SeatComp : SeatComps)
    {
        Seats.Add(SeatComp);
    }

    SeatOccupancy.Empty();
    for (USeatComponent* SC : Seats)
    {
        FSeat Entry;
        Entry.SeatComponent      = SC;
        Entry.OccupantController = nullptr;
        Entry.OriginalPawn       = nullptr;
        
        SeatOccupancy.Add(Entry);

        if (SC->bIsDriverSeat)
        {
            SeatDriver = SC;
        }
    }
}

void AVehicleMaster::InitializeTurrets()
{
    if (!bHaveTurret)
        return;
    
    TArray<UStaticMeshComponent*> SmTurretComps;
    for (UActorComponent* Comp : GetComponentsByTag(UStaticMeshComponent::StaticClass(), TEXT("SmTurrets")))
    {
        if (auto* Mesh = Cast<UStaticMeshComponent>(Comp))
            SmTurretComps.Add(Mesh);
    }

    TArray<UTurretCamera*> CamComps;
    GetComponents<UTurretCamera>(CamComps);

    for (UTurretCamera* CamComp : CamComps)
    {
        if (!CamComp) 
            continue;
        
        ACameraVehicle* TV = CamComp->SpawnCamera(UTurretCamera::StaticClass());
        
        FName SocketName = CamComp->SkeletalSocketName;
        int SeatIdx      = CamComp->AssociateSeatIndex;
        
        CamComp->DestroyComponent();
        if (!TV) continue;

        Turrets.Add(TV);
        USceneComponent* ParentSeat = nullptr;
        UStaticMeshComponent* ParentMeshComp = nullptr;
        
        // --- Skeletal Mesh
        if (SkeletalBaseVehicle)
        {
            TV->AttachToComponent(SkeletalBaseVehicle, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);

            if (Seats.IsValidIndex(SeatIdx))
                ParentSeat = Seats[SeatIdx];
            
        }
        // --- Static Mesh
        else if (SmTurretComps.IsValidIndex(CamComps.IndexOfByKey(CamComp)))
        {
            ParentMeshComp = SmTurretComps[CamComps.IndexOfByKey(CamComp)];
            TV->AttachToComponent(ParentMeshComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

            if (Seats.IsValidIndex(SeatIdx))
                ParentSeat = Seats[SeatIdx];
        }
        // --- Other
        else
        {
            TV->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
        }

        
        if (ParentSeat)
        {
            FTurrets NewEntry;
            NewEntry.Camera     = TV;
            NewEntry.SeatOwner  = ParentSeat;
            NewEntry.TurretMesh = ParentMeshComp;
            NewEntry.BasePitch  = ParentMeshComp->GetRelativeRotation().Pitch;
            NewEntry.BaseYaw    = ParentMeshComp->GetRelativeRotation().Yaw;
            
            SeatTurrets.Add_GetRef(NewEntry);
        }

        if (AnimInstance)
        {
            const FName Socket = TV->GetAttachParentSocketName();
            if (Socket != NAME_None)
            {
                AnimInstance->TurretAngle.FindOrAdd(Socket) = FRotator::ZeroRotator;
            }
        }
    }
}


void AVehicleMaster::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    // --- Camera
    if (bCanRotateCamera)
    {
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
    }


    // --- Turrets
    if (HasAuthority() && !SeatTurrets.IsEmpty())
    {
        for (FTurrets& T : SeatTurrets)
        {
            if (!T.Occupant || !T.Camera)
                continue;

            const FRotator CamRot  = T.Occupant->GetControlRotation();
            const FRotator BaseRot = T.SeatOwner->GetComponentRotation();

            const float DesiredYaw   = FMath::FindDeltaAngleDegrees(BaseRot.Yaw,   CamRot.Yaw);
            const float DesiredPitch = FMath::FindDeltaAngleDegrees(BaseRot.Pitch, CamRot.Pitch);

            const float YawError   = DesiredYaw   - T.AccumulatedYaw;
            const float PitchError = DesiredPitch - T.AccumulatedPitch;

            if (FMath::Abs(YawError) < RotationThreshold && FMath::Abs(PitchError) < RotationThreshold)
            {
                continue;
            }

            ApplyTurretRotation(TurretRotationSpeed, DeltaSeconds, T);
        }
    }

    // --- Sounds
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

bool AVehicleMaster::Interact_Implementation(ACustomPlayerController* PC, USceneComponent* ChosenSeat)
{
    IVehiclesInteractions::Interact_Implementation(PC, ChosenSeat);

    if (!HasAuthority() || !PC || !ChosenSeat)
        return false;
    
    if (CurrentSeats >= MaxSeat)
        return false;

    APawn* Pawn = PC->GetPawn();
    if (!Pawn)
        return false;
    
    if (GetRoleByPlayer(Pawn) != EVehiclePlaceType::None)
        return false;

    FSeat* SeatEntry = SeatOccupancy.FindByPredicate([ChosenSeat](const FSeat& S)
    {
        return S.SeatComponent == ChosenSeat;
    });
    
    if (!SeatEntry || SeatEntry->OccupantController)
        return false;

    // --- Cas Pilote ---
    if (ChosenSeat == SeatDriver)
    {
        if (GetPlayerByRole(EVehiclePlaceType::Driver))
            return false;

        CurrentDriver = Pawn;
        AssignRole(Pawn, EVehiclePlaceType::Driver);

        EnterVehicle(PC, ChosenSeat);
        PC->Possess(this);

        OnEnterDelegate.Broadcast();
        
        return true;
    }
    
    // --- Cas Gunner / Passager ---
    if (!bHaveTurret)
        return false;

    AssignRole(Pawn, EVehiclePlaceType::Gunner);

    EnterVehicle(PC, ChosenSeat);
    PC->Client_AddMappingContext(NewMappingContext);

    OnEnterDelegate.Broadcast();
    
    return true;
}

void AVehicleMaster::OutOfVehicle_Implementation(ACustomPlayerController* PC)
{
    IVehiclesInteractions::OutOfVehicle_Implementation(PC);

    if (!HasAuthority()) 
        return;

    APawn* Pawn = PC->GetPawn();
    if (!Pawn) 
        return;

    FVector ExitLocation = FindClosestExitLocation(PC);

    OnExitDelegate.Broadcast();
    
    // ----- Cas 1 : Pilote -----
    if (Pawn == this && CurrentDriver)
    {
        ExitVehicle(PC);

        CurrentDriver->SetActorLocation(ExitLocation);
        ReleaseRole(CurrentDriver);
        
        CurrentDriver = nullptr;
        ForwardInput = TurnInput = 0.f;
        
        return;
    }

    // ----- Cas 2 : Gunner / autres places -----
    for (FSeat& S : SeatOccupancy)
    {
        if (S.OccupantController == PC)
        {
            ExitVehicle(PC);

            Pawn->SetActorLocation(ExitLocation);
            ReleaseRole(Pawn);
            
            return;
        }
    }
}

void AVehicleMaster::Input_OnExit()
{
  if (ACustomPlayerController* PC = Cast<ACustomPlayerController>(GetController()))
  {
      PC->ExitVehicle();
  } 
}

#pragma endregion


// ------------------- Roles Management -------------------
#pragma region Roles Management

void AVehicleMaster::AssignRole(APawn* Player, EVehiclePlaceType RoleName)
{
    if (!Player)
        return;

    bool bAlreadyAssigned = VehicleRoles.ContainsByPredicate(
        [RoleName](const FVehicleRole& R)
        {
            return R.RoleName == RoleName;
        }
    );

    if (!bAlreadyAssigned)
    {
        VehicleRoles.Add({ Player, RoleName });
    }
}

void AVehicleMaster::ReleaseRole(APawn* Player)
{
    if (!Player)
        return;

    VehicleRoles.RemoveAll(
        [Player](const FVehicleRole& R)
        {
            return R.Player == Player;
        }
    );
}

APawn* AVehicleMaster::GetPlayerByRole(EVehiclePlaceType RoleName) const
{
    const FVehicleRole* Found = VehicleRoles.FindByPredicate(
        [RoleName](const FVehicleRole& R)
        {
            return R.RoleName == RoleName;
        }
    );
    return Found ? Found->Player : nullptr;
}

EVehiclePlaceType AVehicleMaster::GetRoleByPlayer(const APawn* Player) const
{
    const FVehicleRole* Found = VehicleRoles.FindByPredicate(
        [Player](const FVehicleRole& R)
        {
            return R.Player == Player;
        }
    );
    return Found ? Found->RoleName : EVehiclePlaceType::None;
}

#pragma endregion


// ------------------- Enter/Exit -------------------
#pragma region Enter/Exit Player

void AVehicleMaster::EnterVehicle(ACustomPlayerController* PC, USceneComponent* Seat)
{
    if (!PC || !Seat) return;

    FSeat* S = SeatOccupancy.FindByPredicate( [Seat](const FSeat& E)
        { return E.SeatComponent == Seat; }
    );
    if (!S) return;

    S->OccupantController = PC;
    S->OriginalPawn       = PC->GetPawn();
    ++CurrentSeats;

    ACharacter* C = Cast<ACharacter>(PC->GetPawn());
    Multicast_OnEnterSeat(C, Seat);
    
    PC->SetViewTargetWithBlend(this, 0.2f);

    for (FTurrets& T : SeatTurrets)
    {
        if (T.SeatOwner == Seat) T.Occupant = PC;
    }
}

void AVehicleMaster::Multicast_OnEnterSeat_Implementation(ACharacter* Char, USceneComponent* Seat)
{
<<<<<<< Updated upstream
    if (!HasAuthority())
    {
        Server_SwitchToCamera(PlayerController, NewCamera);
        return;
    }
    
    if (AllCameras.IsEmpty() || !PlayerController || !NewCamera || NewCamera->GetIsUsed())
        return;
    
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
=======
    if (!Char || !Seat) return;
    Char->SetActorEnableCollision(false);
    Char->GetCharacterMovement()->DisableMovement();
    Char->AttachToComponent(Seat, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
 }
>>>>>>> Stashed changes

void AVehicleMaster::ExitVehicle(ACustomPlayerController* PC)
{
    if (!PC) return;

    FSeat* Seat = SeatOccupancy.FindByPredicate(
        [PC](const FSeat& E) { return E.OccupantController == PC; }
    );
    
    if (!Seat) return;
    
    const bool bIsDriver = (PC->GetPawn() == this);
    ACharacter* CharacterToDetach = Cast<ACharacter>(bIsDriver ? CurrentDriver : PC->GetPawn());
    
    if (CharacterToDetach)
        Multicast_OnExitSeat(CharacterToDetach);

    if (Seat->OriginalPawn)
    {
        PC->Possess(Seat->OriginalPawn);
        PC->Client_RemoveMappingContext(NewMappingContext);
        PC->SetViewTargetWithBlend(Seat->OriginalPawn, 0.2f);
    }

    Seat->OccupantController = nullptr;
    Seat->OriginalPawn = nullptr;

    for (FTurrets& T : SeatTurrets)
    {
        if (T.Occupant == PC) T.Occupant = nullptr;
    }

    --CurrentSeats;
}

void AVehicleMaster::Multicast_OnExitSeat_Implementation(ACharacter* Char)
{
    if (!Char) return;
    Char->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    Char->SetActorEnableCollision(true);
    Char->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

FVector AVehicleMaster::FindClosestExitLocation(APlayerController* PC)
{
<<<<<<< Updated upstream
    if (!PlayerController || AllCameras.IsEmpty())
        return nullptr;
=======
>>>>>>> Stashed changes
    
    const FSeat* S = SeatOccupancy.FindByPredicate(
        [PC](const FSeat& E){ return E.OccupantController == PC; }
    );
    
<<<<<<< Updated upstream
    if (this == Player)
    {
        PlayerController->Possess(CurrentDriver);
        PlayerController = Cast<APlayerController>(CurrentDriver->GetController());
    }

    ACameraVehicle* CurrentCam = nullptr;
    if (ACameraVehicle** Found = AssignedCameras.Find(PlayerController))
    {
        CurrentCam = *Found;
    }
    
    for (int32 i = 0; i < AllCameras.Num(); i++)
    {
        ACameraVehicle* Camera = AllCameras[i];
        if (Camera && Camera->GetIsUsed() && Camera->GetCameraController() == PlayerController)
        {
            Camera->SetIsUsed(false);
            Camera->SetController(nullptr);
    
            if (i == AllCameras.Num() - 1 || !GetAvailableCamera(i, CurrentCam))
            {
                if (!CurrentDriver && MainCamera)
                {
                    SwitchToMainCam(PlayerController);
                    return nullptr;
                }
            }
            
            ACameraVehicle* NextAvailableCamera = GetAvailableCamera(i, CurrentCam);
            if (NextAvailableCamera)
            {
                GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, NextAvailableCamera->GetName());
                SwitchToCamera(PlayerController, NextAvailableCamera);
                
                return NextAvailableCamera;
            }
    
            Camera->SetIsUsed(true);
            Camera->SetController(PlayerController);
            return Camera;
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

ACameraVehicle* AVehicleMaster::GetAvailableCamera(int startIndex, ACameraVehicle* CurrentCam)
{
    for (int32 j = 1; j < AllCameras.Num(); j++)
    {
        int32 NextIndex = (startIndex + j) % AllCameras.Num();
        ACameraVehicle* NextCamera = AllCameras[NextIndex];
        
        if (!NextCamera || NextCamera->GetIsUsed())
            continue;
        
        if (CurrentCam && CurrentCam->GetIsTurret() != NextCamera->GetIsTurret() && !CurrentCam->GetSwitchToOtherTypeCam())
            continue;

        
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
=======
    FVector Base = S && S->SeatComponent 
        ? S->SeatComponent->GetComponentLocation()
        : GetActorLocation();
    
>>>>>>> Stashed changes
    UWorld* World = GetWorld();
    
    const float MinOffset = 50.f;       // Distance minimale à tester
    const float MaxOffset = 300.f;      // Distance maximale à tester
    const float StepSize = 25.f;        // Incrément de distance entre chaque test
    const float SphereRadius = 20.f;    // Rayon du trace sphérique

    float BestDistance = FLT_MAX;
    FVector BestLocation = Base;
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
    QueryParams.AddIgnoredActor(S->OriginalPawn);

    for (const FVector& Dir : Directions)
    {
        for (float Offset = MinOffset; Offset <= MaxOffset; Offset += StepSize)
        {
            FVector CandidateLocation = Base + Dir * Offset;
            
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
            DrawDebugLine(World, Base, CandidateLocation, bHit ? FColor::Red : FColor::Green, false, 2.f, 0, 2.f);
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


// ------------------- Camera Management -------------------
#pragma region Cameras

void AVehicleMaster::SwitchViewModeVehicle_Implementation(ACustomPlayerController* PlayerController)
{
    IVehiclesInteractions::SwitchViewModeVehicle_Implementation(PlayerController);

    Server_SwitchViewMode(PlayerController);
}

void AVehicleMaster::Input_SwitchViewMode()
{
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
        Server_SwitchViewMode(PC);
}

void AVehicleMaster::Server_SwitchViewMode_Implementation(APlayerController* PC)
{
    FSeat* S = SeatOccupancy.FindByPredicate( [PC](const FSeat& E)
    {
        return E.OccupantController == PC;
    });
    
    if (!S) return;

    Client_SwitchViewMode(PC, S->OriginalPawn);
    
}

void AVehicleMaster::Client_SwitchViewMode_Implementation(APlayerController* PC, APawn* OriginalPawn)
{
    if (PC->GetViewTarget() == this)
    {
        bCanRotateCamera = false;
        PC->SetViewTarget(OriginalPawn);
    }
    else
    {
        bCanRotateCamera = true;
        PC->SetViewTarget(this);
    }
}

void AVehicleMaster::Input_OnUpdateCameraRotation(const FInputActionValue& InputActionValue)
{
    FVector2D InputVector = InputActionValue.Get<FVector2D>();
    OnMouseMoveDelegate.Broadcast();
    
    if (bCanRotateCamera)
    {
        CameraRotationOffset.Pitch += (InputVector.Y * Sensitivity);
        CameraRotationOffset.Yaw   += (InputVector.X * Sensitivity);

        CameraRotationOffset.Pitch = FMath::Clamp(CameraRotationOffset.Pitch, -80.f, 80.f);   
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

#pragma endregion


// ------------------- Vehicle Controls -------------------
#pragma region Vehicle Control

void AVehicleMaster::Input_SwitchEngine()
{
    Server_SwitchEngine(!bEngineOn);

    OnEngineChangeDelegate.Broadcast(!bEngineOn);
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

    OnVehicleMove.Broadcast(ForwardInput, TurnInput);
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

void AVehicleMaster::ApplyTurretRotation(float RotationSpeed, float DeltaTime, FTurrets& TurretEntry)
{
    if (!TurretEntry.Camera || !TurretEntry.Occupant)
        return;
    
    const FRotator CamRot  = TurretEntry.Occupant->GetControlRotation();
    const FRotator BaseRot = GetActorRotation();
    
    const float DesiredYaw   = FMath::FindDeltaAngleDegrees(BaseRot.Yaw,   CamRot.Yaw);
    const float DesiredPitch = FMath::FindDeltaAngleDegrees(BaseRot.Pitch, CamRot.Pitch);

    const float MaxDeltaThisFrame = RotationSpeed * DeltaTime;

    float YawDelta   = FMath::Clamp(DesiredYaw - TurretEntry.AccumulatedYaw, -MaxDeltaThisFrame, +MaxDeltaThisFrame);
    float PitchDelta = FMath::Clamp(DesiredPitch - TurretEntry.AccumulatedPitch, -MaxDeltaThisFrame, +MaxDeltaThisFrame);

    TurretEntry.AccumulatedYaw   = FMath::Clamp(TurretEntry.AccumulatedYaw + YawDelta, -MaxYawRotation, +MaxYawRotation);
    TurretEntry.AccumulatedPitch = FMath::Clamp(TurretEntry.AccumulatedPitch + PitchDelta, -MaxPitchRotation, +MaxPitchRotation);

    const FRotator NewLocalRot = FRotator(
        TurretEntry.BasePitch + TurretEntry.AccumulatedPitch,
         TurretEntry.BaseYaw   + TurretEntry.AccumulatedYaw,
         0.f);
    
    Multicast_SetTurretRotation(TurretEntry.Camera, TurretEntry.TurretMesh, NewLocalRot);
}

void AVehicleMaster::Multicast_SetTurretRotation_Implementation(ACameraVehicle* Camera, UStaticMeshComponent* TurretMesh, FRotator TurretAngle)
{
    if (!Camera)
        return;

    if (SkeletalBaseVehicle && AnimInstance)
    {
        FName TurretName = Camera->GetAttachParentSocketName();
        AnimInstance->UpdateTurretRotation(TurretAngle, TurretName);
    }
    else if (BaseVehicle)
    {
        if (TurretMesh)
        {
            TurretMesh->SetRelativeRotation(TurretAngle);
        }
    }
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

<<<<<<< Updated upstream

// ------------------- Turret Controls -------------------
#pragma region Turret Control

void AVehicleMaster::OnTurretRotate(FVector2D NewRotation, ACameraVehicle* CameraToRotate)
{
    if (!bHaveTurret || Turrets.IsEmpty() || !CameraToRotate->GetIsTurret())
        return;
    
    ApplyTurretRotation(NewRotation.X, NewRotation.Y, TurretRotationSpeed, GetWorld()->GetTimeSeconds(), CameraToRotate);
}

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
=======
>>>>>>> Stashed changes
