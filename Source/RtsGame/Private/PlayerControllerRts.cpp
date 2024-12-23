#include "PlayerControllerRts.h"

#include "EnhancedInputSubsystems.h"
#include "SoldierRts.h"
#include "Blueprint/UserWidget.h"
#include "Components/CommandComponent.h"
#include "Data/AiData.h"
#include "Data/FormationDataAsset.h"
#include "Engine/AssetManager.h"
#include "Interfaces/Selectable.h"
#include "Net/UnrealNetwork.h"
#include "Widget/HudWidget.h"

APlayerControllerRts::APlayerControllerRts(const FObjectInitializer& ObjectInitializer)
{
}

void APlayerControllerRts::BeginPlay()
{
	Super::BeginPlay();
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	verify((AssetManager = UAssetManager::GetIfInitialized()) != nullptr);

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
	bShowMouseCursor = true;

	CreateFormationData();
	CreateHud();
}

void APlayerControllerRts::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APlayerControllerRts, SelectedActors, COND_OwnerOnly);
	
	DOREPLIFETIME_CONDITION(APlayerControllerRts, CurrentFormation, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(APlayerControllerRts, FormationSpacing, COND_OwnerOnly);
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

void APlayerControllerRts::CommandSelected(FCommandData CommandData)
{
	Server_CommandSelected(CommandData);
}

// Selection
#pragma region Selection

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

TArray<AActor*> APlayerControllerRts::GetSelectedActors()
{
	return SelectedActors;
}

bool APlayerControllerRts::ActorSelected(AActor* ActorToCheck) const
{
	if(ActorToCheck && SelectedActors.Contains(ActorToCheck))
	{
		return true;
	}
	return false;
}

// Server Replication
#pragma region Server Replication

void APlayerControllerRts::Server_CommandSelected_Implementation(FCommandData CommandData)
{
	if (!HasAuthority()) return;

	for (int i = 0; i < SelectedActors.Num(); i++)
	{
		if (ASoldierRts* Soldier = Cast<ASoldierRts>(SelectedActors[i]))
		{
			if (CommandData.Target && ISelectable::Execute_GetCurrentTeam(Soldier) == ISelectable::Execute_GetCurrentTeam(CommandData.Target))
				return;
			
			if (!CommandData.Target)
			{
				CalculateOffset(i, CommandData);
			}
			Soldier->GetCommandComponent()->CommandMoveToLocation(CommandData);
		}
	}
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
				Client_Select(ActorsToSelect[i]);
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
		SelectedActors.Add(ActorToSelect);
		OnRep_Selected();

		Client_Select(ActorToSelect);
	}
}

void APlayerControllerRts::Server_DeSelect_Implementation(AActor* ActorToDeSelect)
{
	if (ActorToDeSelect)
	{
		if(ISelectable* Selectable = Cast<ISelectable>(ActorToDeSelect))
		{
			SelectedActors.Remove(ActorToDeSelect);
			OnRep_Selected();
			
			Client_Deselect(ActorToDeSelect);
			Selectable->Deselect();
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
				Client_Deselect(SelectedActors[i]);
				Selectable->Deselect();
			}
		}
	}

	SelectedActors.Empty();
	OnRep_Selected();
}

void APlayerControllerRts::OnRep_Selected() const
{
	OnSelectedUpdate.Broadcast();
}

#pragma endregion

// Client Replication
#pragma region Client Replication

void APlayerControllerRts::Client_Select_Implementation(AActor* ActorToSelect)
{
	if (ISelectable* Selectable = Cast<ISelectable>(ActorToSelect))
	{
		Selectable->Select();
	}
}

void APlayerControllerRts::Client_Deselect_Implementation(AActor* ActorToDeselect)
{
	if (ISelectable* Selectable = Cast<ISelectable>(ActorToDeselect))
	{
		Selectable->Deselect();
	}
}

#pragma endregion

#pragma endregion

// Formation
#pragma region Formation

UFormationDataAsset* APlayerControllerRts::GetFormationData() const
{
	for (int i = 0; i < FormationData.Num(); ++i)
	{
		if (FormationData[i]->FormationType == CurrentFormation)
		{
			return FormationData[i];
		}
	}
	return nullptr;
}

bool APlayerControllerRts::HasGroupSelection() const
{
	return SelectedActors.Num() > 1;
}

void APlayerControllerRts::CreateHud()
{
	if (HudClass)
	{
		Hud = CreateWidget<UHudWidget>(GetWorld(), HudClass);

		if(Hud) Hud->AddToViewport();
	}
}

void APlayerControllerRts::CreateFormationData()
{
	const FPrimaryAssetType AssetType("FormationData");
	TArray<FPrimaryAssetId> Formations;
	AssetManager->GetPrimaryAssetIdList(AssetType, Formations);

	if (Formations.Num() > 0)
	{
		const TArray<FName> Bundles;
		const FStreamableDelegate FormationDataLoadedDelegate = FStreamableDelegate::CreateUObject(this, &APlayerControllerRts::OnFormationDataLoaded, Formations);
		AssetManager->LoadPrimaryAssets(Formations, Bundles, FormationDataLoadedDelegate);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No assets found for type: %s"), *AssetType.ToString());
	}
}

void APlayerControllerRts::OnFormationDataLoaded(TArray<FPrimaryAssetId> Formations)
{
	for (int i = 0; i < Formations.Num(); ++i)
	{
		if (UFormationDataAsset* FormationDataAsset = Cast<UFormationDataAsset>(AssetManager->GetPrimaryAssetObject(Formations[i])))
		{
			FormationData.Add(FormationDataAsset);
			UE_LOG(LogTemp, Log, TEXT("Loaded formation asset: %s"), *FormationDataAsset->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to load formation asset: %s"), *Formations[i].ToString());
		}
	}
}

void APlayerControllerRts::CalculateOffset(const int Index, FCommandData& CommandData)
{
  if (FormationData.Num() <= 0) return;

    if (const UFormationDataAsset* CurrentFormationData = GetFormationData())
    {
	    FVector Offset = CurrentFormationData->Offset;

    	// Traitement du type de formation
    	switch (CurrentFormationData->FormationType)
    	{
    		case EFormation::Blob:
    			{
    				if (Index != 0)
    				{
    					float Angle = (Index / static_cast<float>(SelectedActors.Num())) * 2 * PI;
	
    					float MinSpacing = FormationSpacing * 0.5f;
    					if (Index % 2 == 0)
    					{
    						MinSpacing = MinSpacing * -1;
    					}
    					const float Radius = FMath::RandRange(MinSpacing, FormationSpacing);
	
    					Offset.X += Radius * FMath::Cos(Angle);
    					Offset.Y += Radius * FMath::Sin(Angle);
    				}
    				break;
    			}
    		default:
    			{
    				if (CurrentFormationData->Alternate)
    				{
    					if (Index % 2 == 0)
    					{
    						Offset.Y = Offset.Y * -1;
    					}
    					Offset *= (FMath::Floor((Index + 1) / 2)) * FormationSpacing;
    				}
    				else
    				{
    					Offset *= Index * FormationSpacing;
    				}
    			}
    	}
	    if (CommandData.Rotation == FRotator::ZeroRotator) 
    	Offset = CommandData.Rotation.RotateVector(Offset);
    	FVector TargetLocation = CommandData.SourceLocation + Offset;
    	CommandData.Location = TargetLocation;
    }
}

bool APlayerControllerRts::IsPositionValid(const FVector& Position)
{
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	FCollisionShape CollisionShape = FCollisionShape::MakeSphere(50.0f);
	TArray<struct FOverlapResult> HitResults;

	GetWorld()->OverlapMultiByChannel(HitResults, Position, FQuat::Identity, ECC_GameTraceChannel2, CollisionShape, QueryParams);
	return HitResults.Num() == 0;
}

// Server Replication
#pragma region Server Replication

void APlayerControllerRts::UpdateFormation(EFormation Formation)
{
	if (!HasAuthority())
	{
		Server_UpdateFormation(Formation);
	}
	else
	{
		CurrentFormation = Formation;
		OnRep_CurrentFormation(); // Appliquer immédiatement sur le serveur
	}
}

void APlayerControllerRts::UpdateSpacing(const float NewSpacing)
{
	if (!HasAuthority())
	{
		Server_UpdateSpacing(NewSpacing);
	}
	else
	{
		FormationSpacing = NewSpacing;
		OnRep_FormationSpacing(); // Appliquer immédiatement sur le serveur
	}
}

void APlayerControllerRts::Server_UpdateFormation_Implementation(EFormation Formation)
{
	CurrentFormation = Formation;
	OnRep_CurrentFormation(); // Réplique sur tous les clients
}

void APlayerControllerRts::Server_UpdateSpacing_Implementation(const float NewSpacing)
{
	FormationSpacing = NewSpacing;
	OnRep_FormationSpacing(); // Réplique sur tous les clients
}

void APlayerControllerRts::OnRep_CurrentFormation()
{
	if (HasGroupSelection() && SelectedActors.IsValidIndex(0))
	{
		CommandSelected(FCommandData(SelectedActors[0]->GetActorLocation(), SelectedActors[0]->GetActorRotation(), ECommandType::CommandMove));
	}
}

void APlayerControllerRts::OnRep_FormationSpacing()
{
	if (HasGroupSelection() && SelectedActors.IsValidIndex(0))
	{
		CommandSelected(FCommandData(SelectedActors[0]->GetActorLocation(), SelectedActors[0]->GetActorRotation(), ECommandType::CommandMove));
	}
}

#pragma endregion

#pragma endregion

// Behavior
#pragma region Behavior

void APlayerControllerRts::UpdateBehavior(const ECombatBehavior NewBehavior)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Red, FString("New combat mode:").Append(UEnum::GetDisplayValueAsText(NewBehavior).ToString()));
	if (!HasAuthority())
	{
		Server_UpdateBehavior(NewBehavior);
	}
	else
	{
		for (AActor* Soldier : SelectedActors)
		{
			if (Soldier->Implements<USelectable>())
			{
				ISelectable::Execute_SetBehavior(Soldier, NewBehavior);
			}
		}
	}
}

void APlayerControllerRts::Server_UpdateBehavior_Implementation(const ECombatBehavior NewBehavior)
{
	for(AActor* Soldier: SelectedActors)
	{
		if (Soldier->Implements<USelectable>())
			ISelectable::Execute_SetBehavior(Soldier, NewBehavior);
	}
}

#pragma endregion
