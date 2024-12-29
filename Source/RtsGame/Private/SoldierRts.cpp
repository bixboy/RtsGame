#include "RtsGame/Public/SoldierRts.h"
#include "AiControllerRts.h"
#include "Components/CommandComponent.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

// Setup
#pragma region Setup

ASoldierRts::ASoldierRts()
{
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AiControllerRtsClass;
	
	CommandComp = CreateDefaultSubobject<UCommandComponent>(TEXT("CommandComponent"));
	
	AreaAttack = CreateDefaultSubobject<USphereComponent>(TEXT("AreaAttack"));
	AreaAttack->SetupAttachment(RootComponent);

	AreaAttack->OnComponentBeginOverlap.AddDynamic(this, &ASoldierRts::OnAreaAttackBeginOverlap);
	AreaAttack->OnComponentEndOverlap.AddDynamic(this, &ASoldierRts::OnAreaAttackEndOverlap);
}

void ASoldierRts::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (CommandComp)
	{
		CommandComp->SetOwnerAIController(Cast<AAiControllerRts>(NewController));
	}
}

void ASoldierRts::BeginPlay()
{
	Super::BeginPlay();
}

void ASoldierRts::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASoldierRts::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

auto ASoldierRts::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const -> void
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASoldierRts, CombatBehavior);
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

bool ASoldierRts::GetIsSelected() const
{
	return Selected;
}

#pragma endregion

// Set & Get AiController
#pragma region Set & Get AiController

void ASoldierRts::SetAIController(AAiControllerRts* AiController)
{
	if (!AiController) return;
	
	AIController = AiController;
}

AAiControllerRts* ASoldierRts::GetAiController() const
{
	if (AIController)
	{
		return AIController;
	}

	return nullptr;
}

UCommandComponent* ASoldierRts::GetCommandComponent() const
{
	if(CommandComp) return CommandComp;

	return nullptr;
}


#pragma endregion

//Movement
#pragma region Movement

void ASoldierRts::CommandMove_Implementation(FCommandData CommandData)
{
	ISelectable::CommandMove_Implementation(CommandData);
	GetCommandComponent()->CommandMoveToLocation(CommandData);
}

#pragma endregion

// Attack
#pragma region Attack

void ASoldierRts::TakeDamage_Implementation(AActor* DamageOwner)
{
	IDamageable::TakeDamage_Implementation(DamageOwner);
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("Take Damage By: %s"), *DamageOwner->GetName()));
}


void ASoldierRts::OnAreaAttackBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor == this) return;
	
	if (OtherActor->Implements<USelectable>())
	{
		if (Execute_GetCurrentTeam(OtherActor) == CurrentTeam) return;
			
		UpdateActorsInArea();
		
		ActorsInRange.AddUnique(OtherActor);
		if (AIController && CombatBehavior == ECombatBehavior::Aggressive && !AIController->GetHaveTarget())
		{
			FCommandData CommandData;
			CommandData.Type = ECommandType::CommandAttack;
			CommandData.Target = OtherActor;
			GetCommandComponent()->CommandMoveToLocation(CommandData);
		}
	}
}

void ASoldierRts::OnAreaAttackEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
										UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor == this) return;
	
	if (OtherActor->Implements<USelectable>())
	{
		if (Execute_GetCurrentTeam(OtherActor) == CurrentTeam) return;
		
		UpdateActorsInArea();
		
		if (ActorsInRange.Contains(OtherActor))
			ActorsInRange.Remove(OtherActor);
		
		if (AIController && AIController->GetHaveTarget() && AIController->GetCurrentCommand().Target == OtherActor)
		{
			AActor* NewTarget = nullptr;
			if (ActorsInRange.Num() > 0)
			{
				NewTarget = ActorsInRange[0];
			}

			if (NewTarget)
			{
				FCommandData NewCommandData;
				NewCommandData.Type = ECommandType::CommandAttack;
				NewCommandData.Target = NewTarget;
				GetCommandComponent()->CommandMoveToLocation(NewCommandData);
			}
			else if (AIController->GetCombatBehavior() == ECombatBehavior::Aggressive)
			{
				AIController->StopAttack();
			}
		}

		if (ActorsInRange.Num() == 0 && AIController)
		{
			if (AIController->GetCombatBehavior() == ECombatBehavior::Aggressive)
				AIController->StopAttack();
		}
	}
}

void ASoldierRts::SetBehavior_Implementation(const ECombatBehavior NewBehavior)
{
	ISelectable::SetBehavior_Implementation(NewBehavior);
	CombatBehavior = NewBehavior;
	
	if (AIController)
	{
		AIController->SetupVariables();
		if (CombatBehavior == ECombatBehavior::Passive)
		{
			AIController->StopAttack();
		}
		else if (CombatBehavior == ECombatBehavior::Aggressive)
		{
			if (ActorsInRange.Num() > 0)
			{
				FCommandData NewCommandData;
				NewCommandData.Type = ECommandType::CommandAttack;
				NewCommandData.Target = ActorsInRange[0];
				GetCommandComponent()->CommandMoveToLocation(NewCommandData);
			}
		}
	}
}

ECombatBehavior ASoldierRts::GetBehavior_Implementation()
{
	return CombatBehavior;
}

bool ASoldierRts::GetIsInAttack_Implementation()
{
	return GetAiController()->GetHaveTarget();
}

void ASoldierRts::UpdateActorsInArea()
{
	TArray<AActor*> ValidActors;
    
	for (AActor* Soldier : ActorsInRange)
	{
		if (Soldier && IsValid(Soldier))
		{
			ValidActors.Add(Soldier);
		}
	}
	ActorsInRange = ValidActors;
}

void ASoldierRts::BeginDestroy()
{
	for (AActor* Soldier : ActorsInRange)
	{
		if (ASoldierRts* SoldierRts = Cast<ASoldierRts>(Soldier))
		{
			SoldierRts->UpdateActorsInArea();
		}
	}
	Super::BeginDestroy();
}

void ASoldierRts::OnRep_CombatBehavior()
{
	OnBehaviorUpdate.Broadcast();
}


// Getter
#pragma region Getter

float ASoldierRts::GetAttackRange() const
{
	return AttackRange;
}

float ASoldierRts::GetAttackCooldown() const
{
	return AttackCooldown;
}

ECombatBehavior ASoldierRts::GetCombatBehavior() const
{
	return CombatBehavior;
}

#pragma endregion

#pragma endregion

// Team
#pragma region Team

ETeams ASoldierRts::GetCurrentTeam_Implementation()
{
	return CurrentTeam;
}

#pragma endregion 
