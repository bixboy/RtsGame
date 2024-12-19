#include "RtsGame/Public/SoldierRts.h"

#include "AiControllerRts.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

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
}

void ASoldierRts::DestinationReached(const FCommandData CommandData)
{
	TargetOrientation = CommandData.Rotation;
	ShouldOrientate = 1;
}

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

