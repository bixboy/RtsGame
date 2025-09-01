#include "Components/SlectionComponent.h"
#include "Blueprint/UserWidget.h"
#include "Data/FormationDataAsset.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Interfaces/Selectable.h"
#include "Net/UnrealNetwork.h"
#include "Units/SoldierRts.h"
#include "Widget/HudWidget.h"


void USelectionComponent::BeginPlay()
{
    Super::BeginPlay();
    OwnerController = Cast<APlayerController>(GetOwner());
}

void USelectionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(USelectionComponent, SelectedActors, COND_OwnerOnly);
    DOREPLIFETIME_CONDITION(USelectionComponent, CurrentFormation, COND_OwnerOnly);
    DOREPLIFETIME_CONDITION(USelectionComponent, FormationSpacing, COND_OwnerOnly);
    DOREPLIFETIME_CONDITION(USelectionComponent, UnitToSpawn, COND_OwnerOnly);
}

void USelectionComponent::CreateHud()
{
    if (HudClass && !Hud)
    {
        Hud = CreateWidget<UHudWidget>(GetWorld(), HudClass);
        if (Hud) Hud->AddToViewport();
    }
}

FHitResult USelectionComponent::GetMousePositionOnTerrain() const
{
    FVector WorldLocation, WorldDirection;
    OwnerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

    FHitResult OutHit;
    GetWorld()->LineTraceSingleByChannel(
        OutHit,
        WorldLocation,
        WorldLocation + WorldDirection * 100000.f,
        ECollisionChannel::ECC_GameTraceChannel1
    );

    return OutHit.bBlockingHit ? OutHit : FHitResult();
}

void USelectionComponent::CommandSelected(FCommandData CommandData)
{
    LastFormationActor = nullptr;
    Server_CommandSelected(CommandData);
}


// ------------------- Selection ---------------------

void USelectionComponent::Handle_Selection(AActor* ActorToSelect)
{
    if (!ActorToSelect)
    {
        Server_ClearSelected();
        return;
    }

    if (ActorSelected(ActorToSelect))
    {
        Server_DeSelect(ActorToSelect);   
    }
    else if (ActorToSelect->Implements<USelectable>())
    {
        Server_Select(ActorToSelect);   
    }
    else
        Server_ClearSelected();
}

void USelectionComponent::Handle_Selection(TArray<AActor*> ActorsToSelect)
{
    Server_Select_Group(ActorsToSelect);
}

bool USelectionComponent::ActorSelected(AActor* ActorToCheck) const
{
    return ActorToCheck && SelectedActors.Contains(ActorToCheck);
}

TArray<AActor*> USelectionComponent::GetSelectedActors() const
{
    return SelectedActors;
}


// ------------------- Server Replication ---------------------

void USelectionComponent::Server_CommandSelected_Implementation(FCommandData CommandData)
{
    if (!GetOwner()->HasAuthority()) return;

    for (int32 i = 0; i < SelectedActors.Num(); i++)
    {
        AActor* Soldier = SelectedActors[i];
        if (!Soldier || !Soldier->Implements<USelectable>()) continue;

        // Vérif combat
        if (CommandData.Target &&
            (!ISelectable::Execute_GetCanAttack(Soldier) ||
             ISelectable::Execute_GetCurrentTeam(Soldier) == ISelectable::Execute_GetCurrentTeam(CommandData.Target)))
        {
            continue;
        }

        // Calcul offset formation
        if (!CommandData.Target)
        {
            CalculateOffset(i, CommandData);
        }

        ISelectable::Execute_CommandMove(Soldier, CommandData);
    }
}

void USelectionComponent::Server_Select_Group_Implementation(const TArray<AActor*>& ActorsToSelect)
{
    Server_ClearSelected();

    TArray<AActor*> ValidatedActors;
    ESelectionType BaseType = ESelectionType::None;

    for (AActor* Actor : ActorsToSelect)
    {
        if (!Actor || !Actor->Implements<USelectable>()) continue;

        ESelectionType Type = ISelectable::Execute_GetSelectionType(Actor);
        if (ValidatedActors.IsEmpty())
        {
            BaseType = Type;
            ValidatedActors.Add(Actor);
            Client_Select(Actor);
        }
        else if (Type == BaseType)
        {
            ValidatedActors.Add(Actor);
            Client_Select(Actor);
        }
    }

    if (!ValidatedActors.IsEmpty())
    {
        SelectedActors.Append(ValidatedActors);
        OnRep_Selected();
    }
}

void USelectionComponent::Server_Select_Implementation(AActor* ActorToSelect)
{
    Server_ClearSelected();
    if (ActorToSelect && ActorToSelect->Implements<USelectable>())
    {
        SelectedActors.Add(ActorToSelect);
        OnRep_Selected();
        Client_Select(ActorToSelect);
    }
}

void USelectionComponent::Server_DeSelect_Implementation(AActor* ActorToDeSelect)
{
    if (!ActorToDeSelect || !ActorToDeSelect->Implements<USelectable>()) return;

    SelectedActors.Remove(ActorToDeSelect);
    OnRep_Selected();

    Client_Deselect(ActorToDeSelect);
    Cast<ISelectable>(ActorToDeSelect)->Deselect();
}

void USelectionComponent::Server_ClearSelected_Implementation()
{
    for (AActor* Actor : SelectedActors)
    {
        if (Actor && Actor->Implements<USelectable>())
        {
            Client_Deselect(Actor);
            Cast<ISelectable>(Actor)->Deselect();
        }
    }

    SelectedActors.Empty();
    OnRep_Selected();
}

void USelectionComponent::OnRep_Selected() const
{
    OnSelectedUpdate.Broadcast(SelectedActors);
}


// ------------------- Client Replication ---------------------

void USelectionComponent::Client_Select_Implementation(AActor* ActorToSelect)
{
    if (ISelectable* Selectable = Cast<ISelectable>(ActorToSelect))
    {
        Selectable->Select();
    }
}

void USelectionComponent::Client_Deselect_Implementation(AActor* ActorToDeselect)
{
    if (ISelectable* Selectable = Cast<ISelectable>(ActorToDeselect))
    {
        Selectable->Deselect();
    }
}


// ------------------- Formation ---------------------

bool USelectionComponent::HasGroupSelection() const
{
    return SelectedActors.Num() > 1;
}

UFormationDataAsset* USelectionComponent::GetFormationData() const
{
    return FormationData.FindByPredicate([this](const UFormationDataAsset* Data)
    {
        return Data && Data->FormationType == CurrentFormation;
    }) ? *FormationData.FindByPredicate([this](const UFormationDataAsset* Data)
    {
        return Data && Data->FormationType == CurrentFormation;
    }) : nullptr;
}

void USelectionComponent::CalculateOffset(int Index, FCommandData& CommandData)
{
    if (FormationData.IsEmpty()) return;

    CurrentFormationData = GetFormationData();
    if (!CurrentFormationData) return;

    FVector Offset = CurrentFormationData->Offset;
    const int32 NumActors = SelectedActors.Num();

    switch (CurrentFormationData->FormationType)
    {
        case EFormation::Square:
        {
            const int GridSize = FMath::CeilToInt(FMath::Sqrt((float)NumActors));
            Offset.X = (Index / GridSize) * FormationSpacing - ((GridSize - 1) * FormationSpacing * 0.5f);
            Offset.Y = (Index % GridSize) * FormationSpacing - ((GridSize - 1) * FormationSpacing * 0.5f);
            break;
        }
        case EFormation::Blob:
        {
            if (Index != 0)
            {
                const float Angle = (Index / (float)NumActors) * TWO_PI;
                const float Radius = FMath::RandRange(-FormationSpacing * 0.5f, FormationSpacing * 0.5f);
                Offset.X += Radius * FMath::Cos(Angle);
                Offset.Y += Radius * FMath::Sin(Angle);
            }
            break;
        }
        default:
        {
            const float Multiplier = FMath::Floor((Index + 1) / 2) * FormationSpacing;
            Offset.Y = CurrentFormationData->Alternate && Index % 2 == 0 ? -Offset.Y : Offset.Y;
            Offset *= CurrentFormationData->Alternate ? Multiplier : Index * FormationSpacing;
            break;
        }
    }

    if (!CommandData.Rotation.IsZero())
    {
        Offset = CommandData.Rotation.RotateVector(Offset);
    }

    CommandData.Location = CommandData.SourceLocation + Offset;
}

void USelectionComponent::UpdateFormation(EFormation Formation)
{
    if (GetOwner()->HasAuthority())
    {
        CurrentFormation = Formation;
        OnRep_CurrentFormation();
    }
    else
    {
        Server_UpdateFormation(Formation);
    }
}

void USelectionComponent::UpdateSpacing(float NewSpacing)
{
    if (GetOwner()->HasAuthority())
    {
        FormationSpacing = NewSpacing;
        OnRep_FormationSpacing();
    }
    else
    {
        Server_UpdateSpacing(NewSpacing);
    }
}

void USelectionComponent::Server_UpdateFormation_Implementation(EFormation Formation)
{
    CurrentFormation = Formation;
    OnRep_CurrentFormation();
}

void USelectionComponent::Server_UpdateSpacing_Implementation(float NewSpacing)
{
    FormationSpacing = NewSpacing;
    OnRep_FormationSpacing();
}

void USelectionComponent::OnRep_CurrentFormation()
{
    RefreshFormation(false);
}

void USelectionComponent::OnRep_FormationSpacing()
{
    RefreshFormation(true);
}

void USelectionComponent::RefreshFormation(bool bIsSpacing)
{
    if (!HasGroupSelection() || !SelectedActors.IsValidIndex(0)) return;

    FVector Centroid = FVector::ZeroVector;
    for (AActor* Actor : SelectedActors)
    {
        Centroid += Actor->GetActorLocation();
    }
    Centroid /= SelectedActors.Num();

    LastFormationLocation = Centroid;

    const FRotator PlayerRotation = OwnerController->GetPawn()->GetActorRotation();
    const FRotator CommandRotation(PlayerRotation.Pitch, PlayerRotation.Yaw, 0.f);

    Server_CommandSelected(FCommandData(OwnerController, LastFormationLocation, CommandRotation, ECommandType::CommandMove));
}


// ------------------- Behavior ---------------------

void USelectionComponent::UpdateBehavior(ECombatBehavior NewBehavior)
{
    if (!GetOwner()->HasAuthority())
    {
        Server_UpdateBehavior(NewBehavior);
        return;
    }

    for (AActor* Soldier : SelectedActors)
    {
        if (Soldier->Implements<USelectable>())
        {
            ISelectable::Execute_SetBehavior(Soldier, NewBehavior);
        }
    }
}

void USelectionComponent::Server_UpdateBehavior_Implementation(ECombatBehavior NewBehavior)
{
    UpdateBehavior(NewBehavior);
}


// ------------------- Spawn Units ---------------------

void USelectionComponent::SpawnUnits()
{
    const FHitResult HitResult = GetMousePositionOnTerrain();
    if (HitResult.bBlockingHit)
    {
        Server_SpawnUnits(HitResult.Location);
    }
}

void USelectionComponent::ChangeUnitClass_Implementation(TSubclassOf<ASoldierRts> UnitClass)
{
    UnitToSpawn = UnitClass;
}

void USelectionComponent::OnRep_UnitClass()
{
    OnUnitUpdated.Broadcast(UnitToSpawn);
}

void USelectionComponent::Server_SpawnUnits_Implementation(FVector HitLocation)
{
    if (!UnitToSpawn) return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = OwnerController;
    SpawnParams.Instigator = OwnerController->GetPawn();

    GetWorld()->SpawnActor<ASoldierRts>(UnitToSpawn, HitLocation, FRotator::ZeroRotator, SpawnParams);
}
