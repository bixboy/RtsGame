// ===== File: WorkerComponent.cpp =====

#include "Components/WorkerComp/WorkerComponent.h"
#include "AIController.h"
#include "Components/WorkerComp/TaskManagerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/RtsPlayerController.h"
#include "Units/AI/AiControllerRts.h"


UWorkerComponent::UWorkerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UWorkerComponent::BeginPlay()
{
    Super::BeginPlay();

    if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
    {
        RPC = Cast<ARtsPlayerController>(PC);
        TaskManagerRef = PC->FindComponentByClass<UTaskManagerComponent>();
        
        if (TaskManagerRef)
        {
            TaskManagerRef->RegisterIdleWorker(GetOwner());
            ServerRegisterManager(TaskManagerRef);
        }
    }

    if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
        AIController = Cast<AAiControllerRts>(OwnerPawn->GetController());

    ResourcesComp = GetOwner()->GetComponentByClass<URtsResourcesComponent>();
}

void UWorkerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (TaskManagerRef && !bBusy)
        TaskManagerRef->UnregisterIdleWorker(GetOwner());

    Super::EndPlay(EndPlayReason);
}

void UWorkerComponent::ServerRegisterManager_Implementation(UTaskManagerComponent* ManagerRef)
{
    TaskManagerRef = ManagerRef;
}


void UWorkerComponent::NotifyAssignedJob(const FTaskJob& Job)
{
    StopAll(Job);
    
    CurrentJob = Job;
    bBusy = true;

    switch (Job.TaskType)
    {
    case ETaskType::Collect:
        StartCollect(Job.Target);
        break;
        
    case ETaskType::Build:
        StartBuild(Job.Target);
        break;
        
    case ETaskType::Deliver:
        StartDeliver(Job.Target);
        break;
        
    case ETaskType::Attack:
        StartAttack(Job.Target);
        break;
        
    default:
        break;
    }
}

void UWorkerComponent::MarkJobFinished()
{
    bBusy = false;
    CurrentJob = FTaskJob();

    if (TaskManagerRef)
        TaskManagerRef->RegisterIdleWorker(GetOwner());
}


// ---------------- Movement system ----------------
void UWorkerComponent::MoveToTarget(AActor* Target)
{
    if (!Target || !GetOwner()->HasAuthority()) return;
    
    CurrentTarget = Target;
    bIsMoving = true;

    if (AIController)
    {
        UE_LOG(LogTemp, Log, TEXT("%s -> Start move to %s"), *GetOwner()->GetName(), *GetNameSafe(Target));
        AIController->MoveToLocation(Target->GetActorLocation(), 10.f); // 5 unités de tolérance
    }
}

void UWorkerComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!GetOwner()->HasAuthority()) return;
    
    if (!bIsMoving || !CurrentTarget)
        return;

    FVector OwnerLoc = GetOwner()->GetActorLocation();
    FVector TargetLoc = CurrentTarget->GetActorLocation();

    FBox TargetBox = CurrentTarget->GetComponentsBoundingBox();
    float TargetRadius = FMath::Max3(TargetBox.GetExtent().X, TargetBox.GetExtent().Y, TargetBox.GetExtent().Z);
    float StopThreshold = TargetRadius + 100.f;

    if (FVector::DistSquared(OwnerLoc, TargetLoc) <= StopThreshold * StopThreshold)
    {
        
        bIsMoving = false;

        if (AIController)
            AIController->StopMovement();

        OnArrivedAtTarget(CurrentTarget);
        
    }
}

void UWorkerComponent::OnArrivedAtTarget(AActor* Target)
{
    UE_LOG(LogTemp, Log, TEXT("%s -> Is Arrived At %s"), *GetOwner()->GetName(), *GetNameSafe(Target));
    
    // Worker générique : rien par défaut
}


// ---------------- Callback functions ----------------
void UWorkerComponent::StartCollect(AActor* Target)
{
    UE_LOG(LogTemp, Log, TEXT("%s -> StartCollect on %s"), *GetOwner()->GetName(), *GetNameSafe(Target));
    // TODO : logiques de déplacement, récolte...
    // MarkJobFinished();
}

void UWorkerComponent::StartBuild(AActor* Target)
{
    UE_LOG(LogTemp, Log, TEXT("%s -> StartBuild on %s"), *GetOwner()->GetName(), *GetNameSafe(Target));
    // TODO : logiques de déplacement, construction...
    // MarkJobFinished();
}

void UWorkerComponent::StartDeliver(AActor* Target)
{
    UE_LOG(LogTemp, Log, TEXT("%s -> StartDeliver on %s"), *GetOwner()->GetName(), *GetNameSafe(Target));
    // TODO : logiques de livraison...
    MarkJobFinished();
}

void UWorkerComponent::StartAttack(AActor* Target)
{
    UE_LOG(LogTemp, Log, TEXT("%s -> StartAttack on %s"), *GetOwner()->GetName(), *GetNameSafe(Target));
    // TODO : logiques d’attaque...
    MarkJobFinished();
}


void UWorkerComponent::StopAll(const FTaskJob CommandData)
{
    // Worker générique : rien par défaut
}