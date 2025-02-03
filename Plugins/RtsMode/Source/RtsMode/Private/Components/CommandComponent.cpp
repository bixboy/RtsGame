#include "Components/CommandComponent.h"
#include "Units/AI/AiControllerRts.h"
#include "Units/SoldierRts.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

UCommandComponent::UCommandComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCommandComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerActor = Cast<ASoldierRts>(GetOwner());
	if (OwnerActor)
	{
		OwnerCharaMovementComp = OwnerActor->GetCharacterMovement();
	}

	InitializeMovementComponent();
}

void UCommandComponent::InitializeMovementComponent() const
{
	if (!OwnerCharaMovementComp) return;

	OwnerCharaMovementComp->bOrientRotationToMovement = true;
	OwnerCharaMovementComp->RotationRate = FRotator(0.f, 640.f, 0.f);
	OwnerCharaMovementComp->bConstrainToPlane = true;
	OwnerCharaMovementComp->bSnapToPlaneAtStart = true;
}

void UCommandComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (OwnerActor->HasAuthority() && ShouldOrientate)
	{
		SetOrientation(DeltaTime);
		if (IsOrientated())
		{
			ShouldOrientate = false;
		}
	}
}

// Create & Start Commands
#pragma region Create & Start Commands

void UCommandComponent::SetOwnerAIController(AAiControllerRts* Controller)
{
	OwnerAIController = Controller;
}

FVector UCommandComponent::GetCommandLocation() const
{
	return TargetLocation;
}

void UCommandComponent::CommandMoveToLocation(const FCommandData CommandData)
{
	TargetLocation = CommandData.Location;

	switch (CommandData.Type)
	{
	case ECommandType::CommandMoveSlow:
		SetWalk();
		break;
	case ECommandType::CommandMoveFast:
		SetSprint();
		break;
	case ECommandType::CommandAttack:
		HaveTargetAttack = true;
		SetSprint();
		TargetLocation = CommandData.Target->GetActorLocation();
		break;
	case ECommandType::CommandPatrol:
		SetWalk();
		CommandPatrol(CommandData);
		return;
	default:
		SetRun();
		break;
	}

	CommandMove(CommandData);
}

void UCommandComponent::CommandPatrol(const FCommandData CommandData)
{
	if (OwnerAIController)
	{
		OwnerAIController->OnReachedDestination.Clear();
		OwnerAIController->CommandPatrol(CommandData);
	}
}

void UCommandComponent::CommandMove(const FCommandData CommandData)
{
	if (!OwnerAIController) return;

	OwnerAIController->OnReachedDestination.Clear();
	if (!OwnerAIController->OnReachedDestination.IsBound())
	{
		OwnerAIController->OnReachedDestination.AddDynamic(this, &UCommandComponent::DestinationReached);
	}

	OwnerAIController->CommandMove(CommandData, HaveTargetAttack);
	Client_SetMoveMarker(TargetLocation, CommandData);
	HaveTargetAttack = false;
}

void UCommandComponent::DestinationReached(const FCommandData CommandData)
{
	if (MoveMarker) MoveMarker->Destroy();

	TargetOrientation = CommandData.Rotation;

	if (CommandData.Target)
	{
		TargetOrientation = UKismetMathLibrary::FindLookAtRotation(OwnerActor->GetActorLocation(), CommandData.Target->GetActorLocation());
	}

	ShouldOrientate = true;
}

#pragma endregion

// Walk Speed
#pragma region Walk Speed

void UCommandComponent::SetWalk() const
{
	if (OwnerCharaMovementComp)
	{
		OwnerCharaMovementComp->MaxWalkSpeed = MaxSpeed * 0.5f;
	}
}

void UCommandComponent::SetRun() const
{
	if (OwnerCharaMovementComp)
	{
		OwnerCharaMovementComp->MaxWalkSpeed = MaxSpeed;
	}
}

void UCommandComponent::SetSprint() const
{
	if (OwnerCharaMovementComp)
	{
		OwnerCharaMovementComp->MaxWalkSpeed = MaxSpeed * 1.25f;
	}
}

#pragma endregion

// Orientation
#pragma region Orientation

void UCommandComponent::SetOrientation(float DeltaTime)
{
	if (!OwnerActor) return;

	FRotator InterpolatedRotation = UKismetMathLibrary::RInterpTo(
		FRotator(OwnerActor->GetActorRotation().Pitch, OwnerActor->GetActorRotation().Yaw, 0.f),
		TargetOrientation,
		DeltaTime,
		2.f
	);

	OwnerActor->SetActorRotation(InterpolatedRotation);
}

bool UCommandComponent::IsOrientated() const
{
	if (!OwnerActor) return false;

	const FRotator CurrentRotation = OwnerActor->GetActorRotation();
	return FMath::IsNearlyEqual(CurrentRotation.Yaw, TargetOrientation.Yaw, 0.25f);
}

#pragma endregion

// Marker
#pragma region Marker

// Spawn Marker
void UCommandComponent::Client_SetMoveMarker_Implementation(const FVector Location, const FCommandData CommandData)
{
	if (MoveMarkerClass)
	{
		if (MoveMarker) MoveMarker->Destroy();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Instigator = OwnerActor;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		if (UWorld* World = GetWorld())
		{
			MoveMarker = World->SpawnActor<AActor>(MoveMarkerClass, GetPositionTransform(Location), SpawnParams);
			if (HaveTargetAttack && CommandData.Target)
			{
				MoveMarker->AttachToActor(CommandData.Target, FAttachmentTransformRules::KeepWorldTransform);
			}
		}
	}
}

// Get position For Marker
FTransform UCommandComponent::GetPositionTransform(const FVector Position) const
{
	FHitResult Hit;
	FCollisionQueryParams CollisionParams;
	FVector TraceOrigin = Position + FVector(0.f, 0.f, 10000.f);
	FVector TraceEnd = Position + FVector(0.f, 0.f, -10000.f);

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