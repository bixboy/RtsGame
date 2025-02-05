﻿#include "RtsMode/Public/Player/PlayerCamera.h"
#include "Player/PlayerControllerRts.h"
#include "Player/Selections/SelectionBox.h"
#include "Player/Selections/SphereRadius.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/SlectionComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Interfaces/Selectable.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

APlayerCamera::APlayerCamera()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 2000.0f;
	SpringArm->bDoCollisionTest = false;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArm);
}

void APlayerCamera::BeginPlay()
{
	Super::BeginPlay();

	TargetLocation = GetActorLocation();
	TargetZoom = 3000.f;
	
	const FRotator Rotation = SpringArm->GetRelativeRotation();
	TargetRotation = FRotator(Rotation.Pitch + -50.f,  Rotation.Yaw, 0.f);

	Player = Cast<APlayerControllerRts>(UGameplayStatics::GetPlayerController(GetWorld(), 0.f));

	CreateSelectionBox();
	CreateSphereRadius();
}

void APlayerCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	EdgeScroll();
	CameraBounds();
	
	const FVector InterpolatedLocation = UKismetMathLibrary::VInterpTo(GetActorLocation(), TargetLocation, DeltaTime, CameraSpeed);
	SetActorLocation(FVector(InterpolatedLocation.X, InterpolatedLocation.Y, 0.f));

	const float InterpolatedZoom = UKismetMathLibrary::FInterpTo(SpringArm->TargetArmLength, TargetZoom, DeltaTime, ZoomSpeed);
	SpringArm->TargetArmLength = InterpolatedZoom;

	const FRotator InterpolatedRotation = UKismetMathLibrary::RInterpTo(SpringArm->GetRelativeRotation(), TargetRotation, DeltaTime, CameraSpeed);
	SpringArm->SetRelativeRotation(InterpolatedRotation);

	if (Player && Player->GetInputKeyTimeDown(EKeys::LeftAlt) || Player->GetInputKeyTimeDown(EKeys::RightAlt))
	{
		bAltIsPressed = true;
	}
	else
	{
		bAltIsPressed = false;
	}
}

APlayerControllerRts* APlayerCamera::GetRtsPlayerController()
{
	return Player;
}

// ------------------- Camera Movement   ---------------------
#pragma region Camera Movement Input

void APlayerCamera::MoveForward(float Value)
{
	if(Value == 0.f) return;

	TargetLocation += SpringArm->GetForwardVector() * Value * CameraSpeed;
	GetTerrainPosition(TargetLocation);
}

void APlayerCamera::MoveRight(float Value)
{
	if(Value == 0.f) return;

	TargetLocation += SpringArm->GetRightVector() * Value * CameraSpeed;
	GetTerrainPosition(TargetLocation);
}

void APlayerCamera::RotateCamera(float Angle)
{
	TargetRotation = UKismetMathLibrary::ComposeRotators(TargetRotation, FRotator(0.f, Angle, 0.f));
}

void APlayerCamera::Zoom(float Value)
{
	if(Value == 0.f) return;

	const float Zoom = Value * 100.f;
	TargetZoom = FMath::Clamp(Zoom + TargetZoom, MinZoom, MaxZoom);
}

void APlayerCamera::EnableRotation(const bool bRotate)
{
	CanRotate = bRotate;
}

#pragma endregion

// ------------------- Camera Utility   ---------------------
#pragma region Movement Utiliti

void APlayerCamera::GetTerrainPosition(FVector& TerrainPosition) const
{
	FHitResult Hit;
	FCollisionQueryParams CollisionParams;
	
	FVector StartLocation = TerrainPosition;
	StartLocation.Z += 10000.f;
	FVector EndLocation = TerrainPosition;
	EndLocation.Z -= 10000.f;

	if (UWorld* WorldContext = GetWorld())
	{
		if (WorldContext->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, ECC_Visibility, CollisionParams))
		{
			TerrainPosition = Hit.ImpactPoint;
		}
	}
}

void APlayerCamera::EdgeScroll()
{
	if (!CanEdgeScroll) return;
	
	if (UWorld* WorldContext = GetWorld())
	{
		FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(WorldContext);
		const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(WorldContext);
		MousePosition = MousePosition * UWidgetLayoutLibrary::GetViewportScale(WorldContext);

		MousePosition.X = MousePosition.X / ViewportSize.X;
		MousePosition.Y = MousePosition.Y / ViewportSize.Y;

		//Right/Left
		if(MousePosition.X > 0.98f && MousePosition.X > 1.f)
		{
			MoveRight(EdgeScrollSpeed);
		}
		else if(MousePosition.X < 0.02f && MousePosition.X < 0.f)
		{
			MoveRight(-EdgeScrollSpeed);	
		}
		//Forward/BackWard
		if(MousePosition.Y > 0.98f && MousePosition.Y > 1.f)
		{
			MoveForward(-EdgeScrollSpeed);
		}
		else if(MousePosition.Y < 0.02f && MousePosition.Y < 0.f)
		{
			MoveForward(EdgeScrollSpeed);
		}	
	}
}

void APlayerCamera::CameraBounds()
{
	float NewPitch = TargetRotation.Pitch;
	if(TargetRotation.Pitch < (RotatePitchMax * -1.f))
	{
		NewPitch = (RotatePitchMax * -1.f);
	}
	else if(TargetRotation.Pitch > (RotatePitchMin * -1.f))
	{
		NewPitch = (RotatePitchMin * -1.f);
	}

	TargetRotation = FRotator(NewPitch, TargetRotation.Yaw, 0.f);
}

#pragma endregion

// ------------------- Selection  ---------------------
#pragma region Selection

// Left Click
void APlayerCamera::HandleLeftMouse(EInputEvent InputEvent, float Value)
{
	if (!Player) return;

	switch (InputEvent)
	{
	case IE_Pressed:
		Player->SelectionComponent->Handle_Selection(nullptr);
		BoxSelect = false;
		LeftMouseHitLocation = Player->SelectionComponent->GetMousePositionOnTerrain();
		CommandStart();
		break;

	case IE_Released:
		if (BoxSelect && SelectionBox)
		{
			SelectionBox->End();
			BoxSelect = false;
		}
		else
		{
			Player->SelectionComponent->Handle_Selection(GetSelectedObject());
		}
		break;

	case IE_Repeat:
		if (Value == 0.f)
		{
			SelectionBox->End();
			return;
		}

		if (Player->GetInputKeyTimeDown(EKeys::LeftMouseButton) >= LeftMouseHoldThreshold && SelectionBox)
		{
			if (!BoxSelect)
			{
				SelectionBox->Start(LeftMouseHitLocation, TargetRotation);
				BoxSelect = true;
			}
		}
		break;

	default:
		break;
	}
}

AActor* APlayerCamera::GetSelectedObject()
{
	if (UWorld* World = GetWorld())
	{
		FVector WorldLocation, WorldDirection;

		Player->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);
		FVector End = WorldDirection * 1000000.0f + WorldLocation;

		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this);

		FHitResult Hit;
		if (World->LineTraceSingleByChannel(Hit, WorldLocation, End, ECC_Visibility, CollisionParams))
		{
			if (AActor* HitActor = Hit.GetActor())
			{
				return HitActor;
			}
		}
	}

	return nullptr;
}

void APlayerCamera::CreateSelectionBox()
{
	if(SelectionBox) return;

	if(UWorld* World = GetWorld())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Instigator = this;
		SpawnParams.Owner = this;
		SelectionBox = World->SpawnActor<ASelectionBox>(SelectionBoxClass,FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if(SelectionBox)
		{
			SelectionBox->SetOwner(this);
		}
	}
}

// Alt Click
void APlayerCamera::HandleAltRightMouse(EInputEvent InputEvent, float Value)
{
	if (!Player) return;

	switch (InputEvent)
	{
	case IE_Pressed:
		if (!bAltIsPressed) return;
		LeftMouseHitLocation = Player->SelectionComponent->GetMousePositionOnTerrain();
		break;

	case IE_Released:
		if (SphereRadiusEnable && SphereRadius)
		{
			SphereRadius->End();
			SphereRadiusEnable = false;
			Player->SelectionComponent->CommandSelected(CreateCommandData(ECommandType::CommandPatrol, nullptr, SphereRadius->GetRadius()));
		}
		break;

	case IE_Repeat:  // Correspond au "Hold"
		if (!bAltIsPressed)
		{
			SphereRadius->End();
			SphereRadiusEnable = false;
			return;
		}

		if (Value == 0.f)
		{
			SphereRadius->End();
			return;
		}

		if (Player->GetInputKeyTimeDown(EKeys::RightMouseButton) >= LeftMouseHoldThreshold && SphereRadius)
		{
			if (!SphereRadiusEnable)
			{
				SphereRadius->Start(LeftMouseHitLocation, TargetRotation);
				SphereRadiusEnable = true;
			}
		}
		break;

	default:
		break;
	}
}

void APlayerCamera::CreateSphereRadius()
{
	if(UWorld* World = GetWorld())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Instigator = this;
		SpawnParams.Owner = this;
		SphereRadius = World->SpawnActor<ASphereRadius>(SphereRadiusClass,FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if(SphereRadius)
		{
			SphereRadius->SetOwner(this);
		}
	}
}

#pragma endregion

// ------------------- Command  ---------------------
#pragma region Command

void APlayerCamera::CommandStart()
{
	if (!Player) return;

	FVector MouseLocation = Player->SelectionComponent->GetMousePositionOnTerrain();
	CommandLocation = FVector(MouseLocation.X, MouseLocation.Y, MouseLocation.Z);
}

void APlayerCamera::Command()
{
	if (!Player) return;

	AActor* ActorEnemy = GetSelectedObject();
	if (ActorEnemy && ActorEnemy->Implements<USelectable>())
	{
		Player->SelectionComponent->CommandSelected(CreateCommandData(ECommandType::CommandAttack, ActorEnemy));
		return;
	}
	Player->SelectionComponent->CommandSelected(CreateCommandData(ECommandType::CommandMove));
}

FCommandData APlayerCamera::CreateCommandData(const ECommandType Type, AActor* Enemy, const float Radius) const
{
	if (!Player) return FCommandData();
	
	FRotator CommandRotation = FRotator::ZeroRotator;
	const FVector CommandEndLocation = Player->SelectionComponent->GetMousePositionOnTerrain();

	if ((CommandEndLocation - CommandLocation).Length() > 100.f)
	{
		const FVector Direction = CommandEndLocation - CommandLocation;
		const float RotationAngle = FMath::RadiansToDegrees(FMath::Atan2(Direction.Y, Direction.X));

		CommandRotation = FRotator(0.f, RotationAngle, 0.f);
	}

	if (Type == ECommandType::CommandAttack)
	{
		return FCommandData(CommandLocation, CameraComponent->GetComponentRotation(), Type, Enemy);
	}
	else if (Type == ECommandType::CommandPatrol) 
	{
		return FCommandData(CommandLocation, CameraComponent->GetComponentRotation(), Type, nullptr, Radius * 2.f);
	}
	
	return FCommandData(CommandLocation, CameraComponent->GetComponentRotation(), Type);
}

#pragma endregion




