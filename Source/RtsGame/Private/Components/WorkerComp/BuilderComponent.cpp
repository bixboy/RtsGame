// ===== File: BuilderComponent.cpp =====

#include "Components/WorkerComp/BuilderComponent.h"
#include "Data/WorkerData.h"


UBuilderComponent::UBuilderComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBuilderComponent::StartBuild(AActor* Target)
{
    Super::StartBuild(Target);
    
    if (!Target || !ResourcesComp || !GetOwner()->HasAuthority())
    {
        MarkJobFinished();
        return;
    }

    AStructureBase* Build = Cast<AStructureBase>(Target);
    if (!Build)
    {
        MarkJobFinished();
        return;
    }
    
    // 1️⃣ Vérifie si le bâtiment a déjà toutes ses ressources
    if (Build->GetIsFullyResourced())
    {
        Build->AddWorker();
        bIsBuilding = true;
        
        UE_LOG(LogTemp, Log, TEXT("Builder %s starts building fully resourced structure %s"), *GetOwner()->GetName(), *Build->GetName());
        return;
    }

    // 2️⃣ Vérifie les ressources disponibles
    FResourcesCost Needed, Available;
    Build->GetNeedsResources(Needed);
    Available = Needed.GetClamped(ResourcesComp->GetResources());

    if (Available.HasAnyResource())
    {
        // Livrer ce qu’on peut
        ResourcesComp->RemoveResources(Available);
        Build->DeliverResources(Available);
        Build->AddWorker();
        bIsBuilding = true;
        
        UE_LOG(LogTemp, Log, TEXT("Builder %s delivers resources to %s"), *GetOwner()->GetName(), *Build->GetName());
    }
    else
    {
        // 3️⃣ Aller chercher les ressources
        
        // ATTENTION RISQUE DE BUG CAST !!!!!!!!!!!!!!!!!! 
        SavedConstruction = Cast<AStructureBase>(Target);
        MoveToNearestStorage(Needed);
    }
}

void UBuilderComponent::MoveToBuild(AStructureBase* Build)
{
    if (!Build || TargetBuild == Build)
        return;

    if (!ResourcesComp)
        return;

    TargetBuild = Build;

    // Bâtiment déjà prêt en ressources → direct move
    if (Build->GetIsFullyResourced())
    {
        MoveToTarget(Build);
        bGoingToStorage = false;
        return;
    }

    // Vérifie ressources dispo
    FResourcesCost Needed, Available;
    if (!CheckIfHaveResources(Needed, Available) && !bGoingToStorage && SavedConstruction != TargetBuild)
    {
        MoveToNearestStorage(Needed);
        return;
    }

    // Sinon → on va au bâtiment
    MoveToTarget(Build);
    bGoingToStorage = false;
}

void UBuilderComponent::MoveToNearestStorage(const FResourcesCost& NeededResources)
{
    if (!GetOwner()->HasAuthority() || !TaskManagerRef)
        return;

    AResourceDepot* NearestStorage = TaskManagerRef->FindNearestStorage(GetOwner(), NeededResources);
    if (NearestStorage)
    {
        bGoingToStorage = true;
        MoveToTarget(NearestStorage);
    }
}

void UBuilderComponent::OnArrivedAtTarget(AActor* Target)
{
    if ((!TargetBuild && !SavedConstruction) || !Target->Implements<UBuildInterface>())
    {
        Super::OnArrivedAtTarget(Target);
        return;
    }
    
    if (bGoingToStorage)
    {
        // Récupère ressources
        if (AResourceDepot* Storage = Cast<AResourceDepot>(Target))
        {
            if (SavedConstruction)
            {
                FResourcesCost Needed, Available;
                SavedConstruction->GetNeedsResources(Needed);
                Available = Needed.GetClamped(Storage->GetStorage());

                Storage->RemoveResources(Available);
                ResourcesComp->AddResources(Available);

                bGoingToStorage = false;
                
                MoveToBuild(SavedConstruction);
                SavedConstruction = nullptr;
            }
        }
    }
    else
    {
        // Arrivé sur le bâtiment → commence construction
        StartInteractWithBuild();
    }
}

void UBuilderComponent::StopAll(const FTaskJob CommandData)
{
    Super::StopAll(CommandData);

    if (CommandData.Target == TargetBuild) return;

    TargetBuild = nullptr;
    SavedConstruction = nullptr;
    bIsBuilding = false;
    bGoingToStorage = false;
}

void UBuilderComponent::StartInteractWithBuild()
{
    if (!TargetBuild || !ResourcesComp) return;

    if (TargetBuild->GetIsFullyResourced())
    {
        TargetBuild->AddWorker();
        bIsBuilding = true;
        return;
    }

    FResourcesCost Needed, Available;
    if (CheckIfHaveResources(Needed, Available))
    {
        ResourcesComp->RemoveResources(Available);
        TargetBuild->DeliverResources(Available);
        TargetBuild->AddWorker();
        bIsBuilding = true;
    }
    else
    {
        MoveToNearestStorage(Needed);
    }
}


bool UBuilderComponent::CheckIfHaveResources(FResourcesCost& ResourcesNeed, FResourcesCost& AvailableResources) const
{
    if (!TargetBuild || !ResourcesComp) return false;
    
    FResourcesCost BuildNeeds;
    TargetBuild->GetNeedsResources(BuildNeeds);
	
    AvailableResources = BuildNeeds.GetClamped(ResourcesComp->GetResources());
    ResourcesNeed = BuildNeeds;
	
    return AvailableResources.HasAnyResource();
}