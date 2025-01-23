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

void AVehicleMaster::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AVehicleMaster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// Possess
void AVehicleMaster::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    if (CurrentDriver) return;
    
    if (APlayerController* PlayerController = Cast<APlayerController>(NewController))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(NewMappingContext, 1);
            CurrentDriver = PlayerController->GetCharacter();
        }
    }
}

void AVehicleMaster::UnPossessed()
{
    Super::UnPossessed();
    
}

// Helper Function: Initialize Cameras
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
            else
                SpawnedCamera->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

            // Set Camera Rotation
            SpawnedCamera->SetActorRotation(Cam->GetComponentRotation());

            // Initialize Animation Data
            if (AnimInstance)
                AnimInstance->TurretAngle.Add(SpawnedCamera->GetAttachParentSocketName(), FRotator::ZeroRotator);

            Cam->DestroyComponent();
            CameraIndex++;
        }
    }
}

#pragma endregion

// ------------------- Interaction -------------------
void AVehicleMaster::Interact_Implementation(APawn* PlayerInteract)
{
    IVehiclesInteractions::Interact_Implementation(PlayerInteract);

    PlayerInteract->GetController()->Possess(this);
    if (APlayerController* Oui = Cast<APlayerController>(PlayerInteract->GetController()))
    {
        SwitchToNextCamera(Oui);
    }
}

// ------------------- Camera Management -------------------
#pragma region Cameras

void AVehicleMaster::SwitchToCamera(APlayerController* PlayerController, ACameraVehicle* NewCamera)
{
    if (!PlayerController || !NewCamera || NewCamera->GetIsUsed()) return;

    // Deactivate Current Camera
    for (ACameraVehicle* TempCamera : Cameras)
    {
        if (TempCamera && TempCamera->GetIsUsed() && TempCamera->GetCameraController() == PlayerController)
        {
            TempCamera->SetIsUsed(false);
            TempCamera->SetController(nullptr);
        }
    }

    // Activate New Camera
    NewCamera->SetIsUsed(true);
    NewCamera->SetController(PlayerController);
    CurrentCamera = NewCamera;

    PlayerController->SetViewTargetWithBlend(NewCamera, 0.1f, EViewTargetBlendFunction::VTBlend_Cubic, 0.0f, false);
    UE_LOG(LogTemp, Warning, TEXT("Switched to camera: %s for player: %s"), *NewCamera->GetName(), *PlayerController->GetName());
}

void AVehicleMaster::SwitchToNextCamera(APlayerController* PlayerController)
{
    if (!PlayerController || Cameras.IsEmpty()) return;

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

ACameraVehicle* AVehicleMaster::GeCameraInArray(int Index)
{
    return Cameras.IsValidIndex(Index) ? Cameras[Index] : nullptr;
}

#pragma endregion

// ------------------- Vehicle Controls -------------------
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