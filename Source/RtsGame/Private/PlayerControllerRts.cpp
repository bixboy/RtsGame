#include "PlayerControllerRts.h"

#include "AiData.h"
#include "EnhancedInputSubsystems.h"
#include "SoldierRts.h"
#include "Interfaces/Selectable.h"
#include "Net/UnrealNetwork.h"

APlayerControllerRts::APlayerControllerRts(const FObjectInitializer& ObjectInitializer)
{
}

void APlayerControllerRts::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APlayerControllerRts, SelectedActors, COND_OwnerOnly);
}

FVector APlayerControllerRts::GetMousePositionOnTerrain() const
{
	FVector WorldLocation, WorldDirection;
	DeprojectMousePositionToWorld(WorldLocation, WorldDirection);
	FHitResult OutHit;
	if (GetWorld()->LineTraceSingleByChannel(OutHit,WorldLocation, WorldLocation + (WorldDirection * 100000.f), ECollisionChannel::ECC_GameTraceChannel1))
	{
		if (OutHit.bBlockingHit)
		{
			return OutHit.Location;
		}
	}

	return FVector::ZeroVector;
}

void APlayerControllerRts::Handle_Selection(AActor* ActorToSelect)
{
	if (ISelectable* Selectable = Cast<ISelectable>(ActorToSelect))
	{
		if (ActorToSelect && ActorSelected(ActorToSelect))
		{
			Server_DeSelect(ActorToSelect);
		}
		else
		{
			Server_Select(ActorToSelect);
		}
	}
	else
	{
		Server_ClearSelected();
	}
}

void APlayerControllerRts::Handle_Selection(TArray<AActor*> ActorToSelect)
{
	Server_Select_Group(ActorToSelect);
}

void APlayerControllerRts::BeginPlay()
{
	Super::BeginPlay();
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
	bShowMouseCursor = true;
}

void APlayerControllerRts::CommandSelected(FCommandData CommandData)
{
	Server_CommandSelected(CommandData);
}

void APlayerControllerRts::Server_CommandSelected_Implementation(FCommandData CommandData)
{
	if (!HasAuthority()) return;

	for (int i = 0; i < SelectedActors.Num(); i++)
	{
		if (ASoldierRts* Soldier = Cast<ASoldierRts>(SelectedActors[i]))
		{
			Soldier->CommandMoveToLocation(CommandData);
		}
	}
}

bool APlayerControllerRts::ActorSelected(AActor* ActorToCheck) const
{
	if(ActorToCheck && SelectedActors.Contains(ActorToCheck))
	{
		return true;
	}
	return false;
}

void APlayerControllerRts::Server_Select_Group_Implementation(const TArray<AActor*>& ActorsToSelect)
{
	Server_ClearSelected();

	TArray<AActor*> ValidatedActors;
	for (int i = 0; i < ActorsToSelect.Num(); i++)
	{
		if (ActorsToSelect[i])
		{
			if(ISelectable* Selectable = Cast<ISelectable>(ActorsToSelect[i]))
			{
				ValidatedActors.Add(ActorsToSelect[i]);
				Selectable->Select();
			}
		}
	}


	SelectedActors.Append(ValidatedActors);
	OnRep_Selected();
	ValidatedActors.Empty();
}

void APlayerControllerRts::Server_Select_Implementation(AActor* ActorToSelect)
{
	Server_ClearSelected();

	if(ISelectable* Selectable = Cast<ISelectable>(ActorToSelect))
	{
		Selectable->Select();
		SelectedActors.Add(ActorToSelect);
		OnRep_Selected();
	}
}

void APlayerControllerRts::Server_DeSelect_Implementation(AActor* ActorToDeSelect)
{
	if (ActorToDeSelect)
	{
		if(ISelectable* Selectable = Cast<ISelectable>(ActorToDeSelect))
		{
			Selectable->Deselect();
			SelectedActors.Remove(ActorToDeSelect);
			OnRep_Selected();
		}
	}
}

void APlayerControllerRts::Server_ClearSelected_Implementation()
{
	for (int i = 0; i < SelectedActors.Num(); i++)
	{
		if (SelectedActors[i])
		{
			if (ISelectable* Selectable = Cast<ISelectable>(SelectedActors[i]))
			{
				Selectable->Deselect();
			}
		}
	}

	SelectedActors.Empty();
	OnRep_Selected();
}

void APlayerControllerRts::OnRep_Selected()
{
	OnSelectedUpdate.Broadcast();
}
