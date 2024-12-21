#include "RtsGame/Public/SoldierRts.h"

#include "AiControllerRts.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Setup
#pragma region Setup

ASoldierRts::ASoldierRts()
{
	PrimaryActorTick.bCanEverTick = true;

	CharaMovementComp = GetCharacterMovement();
	if (CharaMovementComp)
	{
		CharaMovementComp->bOrientRotationToMovement = true;
		CharaMovementComp->RotationRate = FRotator(0.f, 640.f, 0.f);
		CharaMovementComp->bConstrainToPlane = true;
		CharaMovementComp->bSnapToPlaneAtStart = true;
	}

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ASoldierRts::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASoldierRts::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ShouldOrientate)
	{
		SetOrientation(DeltaTime);

		if (IsOrientated())
		{
			ShouldOrientate = 0;
		}
	}
}

void ASoldierRts::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

#pragma endregion

// Selection
#pragma region Selection

void ASoldierRts::Select()
{
	Selected = true;
	Highlight(Selected);
}

void ASoldierRts::Deselect()
{
	Selected = false;
	Highlight(Selected);
}

void ASoldierRts::Highlight(const bool Highlight)
{
	TArray<UPrimitiveComponent*> Components;
	GetComponents<UPrimitiveComponent>(Components);
	for(UPrimitiveComponent* VisualComp : Components)
	{
		if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(VisualComp))
		{
			Prim->SetRenderCustomDepth(Highlight);
		}
	}
}

#pragma endregion

// Create & Start Commands
#pragma region Create & Start Commands

void ASoldierRts::CommandMoveToLocation(const FCommandData CommandData)
{
	switch (CommandData.Type)
	{
		case ECommandType::CommandMoveSlow:
		{
			SetWalk();
			break;	
		}
		case ECommandType::CommandMoveFast:
		{
			SetSprint();
			break;	
		}
		case ECommandType::CommandAttack:
		{
			break;
		}
		default:
		{
				SetRun();
		}
	}

	CommandMove(CommandData);
}

void ASoldierRts::SetAIController(AAiControllerRts* AiController)
{
	AIController = AiController;
}

void ASoldierRts::CommandMove(const FCommandData CommandData)
{
	if(!AIController)return;
	
	AIController->OnReachedDestination.Clear();

	if (!AIController->OnReachedDestination.IsBound())
	{
		AIController->OnReachedDestination.AddDynamic(this, &ASoldierRts::DestinationReached);
	}

	AIController->CommandMove(CommandData);
	SetMoveMarker(CommandData.Location);
}

void ASoldierRts::DestinationReached(const FCommandData CommandData)
{
	if(MoveMarker) MoveMarker->Destroy();
	
	TargetOrientation = CommandData.Rotation;
	ShouldOrientate = 1;
}

#pragma endregion

// Walk Speed
#pragma region Walk Speed

void ASoldierRts::SetWalk() const
{
	if (!CharaMovementComp) return;

	CharaMovementComp->MaxWalkSpeed = MaxSpeed * 0.5f;
}

void ASoldierRts::SetRun() const
{
	if (!CharaMovementComp) return;

	CharaMovementComp->MaxWalkSpeed = MaxSpeed;
}

void ASoldierRts::SetSprint() const
{
	if (!CharaMovementComp) return;

	CharaMovementComp->MaxWalkSpeed = MaxSpeed * 1.25f;
}

#pragma endregion

void ASoldierRts::SetOrientation(const float DeltaTime)
{
	const FRotator InterpolatedRotation = UKismetMathLibrary::RInterpTo(FRotator(GetActorRotation().Pitch, GetActorRotation().Yaw, 0.f), TargetOrientation, DeltaTime, 2.f);
	SetActorRotation(InterpolatedRotation);
}

bool ASoldierRts::IsOrientated() const
{
	const FRotator CurrentRotation = GetActorRotation();

	if(FMath::IsNearlyEqual(CurrentRotation.Yaw, TargetOrientation.Yaw, 0.25f))
	{
		return true;
	}
	return false;
}

// Marker
#pragma region Marker

// Spawn Marker
void ASoldierRts::SetMoveMarker(const FVector Location)
{
	if (MoveMarkerClass)
	{
		if (MoveMarker) MoveMarker->Destroy();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Instigator = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		if (UWorld* World = GetWorld())
		{
			MoveMarker = World->SpawnActor<AActor>(MoveMarkerClass, GetPositionTransform(Location), SpawnParams);
		}
	}
}

// Get position For Marker
FTransform ASoldierRts::GetPositionTransform(const FVector Position) const
{
	FHitResult Hit;
	FCollisionQueryParams CollisionParams;
	FVector TraceOrigin = Position;
	TraceOrigin.Z += 10000.f;
	FVector TraceEnd = Position;
	TraceEnd.Z -= 10000.f;

	if (UWorld* World = GetWorld())
	{
		if (World->LineTraceSingleByChannel(Hit, TraceOrigin, TraceEnd, ECollisionChannel::ECC_GameTraceChannel1, CollisionParams))
		{
			if (Hit.bBlockingHit)
			{
				FTransform HitTransform;
				HitTransform.SetLocation(Hit.ImpactPoint + FVector(1.f, 1.f, 1.25f));
				FRotator TerrainRotation = UKismetMathLibrary::MakeRotFromZX(Hit.Normal, FVector::UpVector);
				TerrainRotation += FRotator(90.f, 0.f, 0.f);
				HitTransform.SetRotation(TerrainRotation.Quaternion());
				return HitTransform;
			}
		}
	}
	return FTransform::Identity;
}

#pragma endregion 