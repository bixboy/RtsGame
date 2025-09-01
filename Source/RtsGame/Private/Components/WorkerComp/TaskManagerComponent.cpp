#include "Components/WorkerComp/TaskManagerComponent.h"
#include "Components/RtsComponent.h"
#include "Player/RtsPlayerController.h"
#include "Structures/ResourceDepot.h"
#include "Structures/StructureBase.h"


UTaskManagerComponent::UTaskManagerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
}


void UTaskManagerComponent::BeginPlay()
{
    Super::BeginPlay();

    PC = Cast<APlayerController>(GetOwner());

    if (PC)
        RPC = Cast<ARtsPlayerController>(PC);
}


void UTaskManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    PendingJobs.Empty();
    IdleWorkers.Empty();
    
    Super::EndPlay(EndPlayReason);
}


int32 UTaskManagerComponent::SubmitJob(const FTaskJob& JobIn)
{
    FTaskJob NewJob = JobIn;
    NewJob.JobId = NextJobId++;
    
    if (NewJob.bForced)
        PendingJobs.Insert(MoveTemp(NewJob), 0);
    else
        PendingJobs.Add(MoveTemp(NewJob));
    
    return NewJob.JobId;
}


void UTaskManagerComponent::CancelJob(int32 JobId)
{
    for (int32 i = PendingJobs.Num() - 1; i >= 0; --i)
    {
        if (PendingJobs[i].JobId == JobId)
        {
            PendingJobs.RemoveAt(i);
            return;
        }
    }
}


void UTaskManagerComponent::RegisterIdleWorker(AActor* Worker)
{
    if (!Worker) return;
    IdleWorkers.Add(Worker);
}


void UTaskManagerComponent::UnregisterIdleWorker(AActor* Worker)
{
    if (!Worker) return;
    IdleWorkers.Remove(Worker);
}


void UTaskManagerComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!bAutoWork)
        return;
    
    AssignmentAccumulator += DeltaTime;
    if (AssignmentAccumulator >= AssignmentInterval)
    {
        AssignmentAccumulator = 0.f;
        AssignJobs();
    }
}


void UTaskManagerComponent::AssignJobs()
{
    if (PendingJobs.Num() == 0 || IdleWorkers.Num() == 0) return;

    PendingJobs.StableSort([](const FTaskJob& A, const FTaskJob& B) {
        if (A.bForced != B.bForced)
        {
            return A.bForced > B.bForced;
        }
        
        return A.Priority > B.Priority;
    });

    for (int32 j = PendingJobs.Num() - 1; j >= 0; --j)
    {
        FTaskJob& Job = PendingJobs[j];

        AActor* Best = FindBestWorkerForJob(Job);
        if (!Best) continue;

        IUnitTypeInterface::Execute_StartWork(Best, Job);
        UnregisterIdleWorker(Best);

        Job.RequiredWorkers = FMath::Max(0, Job.RequiredWorkers - 1);
        if (Job.RequiredWorkers <= 0)
        {
            PendingJobs.RemoveAt(j);
        }
    }
}

void UTaskManagerComponent::AssignJobToWorkers(const FTaskJob& Job, const TArray<AActor*> Workers)
{
    if (Workers.Num() == 0) return;

    for (AActor* Worker : Workers)
    {
        if (!Worker) continue;

        FTaskJob JobCopy = Job;
        JobCopy.JobId = NextJobId++;

        IUnitTypeInterface::Execute_StartWork(Worker, JobCopy);
        UnregisterIdleWorker(Worker);
    }
}

void UTaskManagerComponent::AssignJobToWorker(const FTaskJob& Job, AActor* Worker)
{
    if (!Worker) return;

    FTaskJob JobCopy = Job;
    JobCopy.JobId = NextJobId++;

    IUnitTypeInterface::Execute_StartWork(Worker, JobCopy);
    UnregisterIdleWorker(Worker);
}


// ====== Find Best Worker
AActor* UTaskManagerComponent::FindBestWorkerForJob(const FTaskJob& Job) const
{
    if (IdleWorkers.Num() == 0) return nullptr;

    AActor* Best = nullptr;
    float BestDistSq = FLT_MAX;

    FVector TargetLoc = Job.Target ? Job.Target->GetActorLocation() : FVector::ZeroVector;

    for (AActor* W : IdleWorkers)
    {
        if (!W)
            continue;
        
        float DistSq = FVector::DistSquared(W->GetActorLocation(), TargetLoc);
        if (DistSq < BestDistSq)
        {
            BestDistSq = DistSq;
            Best = W;
        }
    }

    return Best;
}


// ====== Find Nearest Storage
AResourceDepot* UTaskManagerComponent::FindNearestStorage(const AActor* Unit, const FResourcesCost& Needed) const
{
    if (!PC) return nullptr;

    if (!RPC || !RPC->RtsComponent) return nullptr;

    AResourceDepot* Nearest = nullptr;
    float NearestDist = FLT_MAX;
    FVector MyLoc = Unit->GetActorLocation();

    for (AStructureBase* Build : RPC->RtsComponent->GetBuilds())
    {
        if (!Build || !Build->GetIsBuilt())
            continue;
        
        AResourceDepot* Storage = Cast<AResourceDepot>(Build);
        if (!Storage)
            continue;

        if (Needed != FResourcesCost())
        {
            FResourcesCost Give = Needed.GetClamped(Storage->GetStorage());
            if (!Give.HasAnyResource())
                continue;   
        }
        
        float Dist = FVector::Dist(MyLoc, Storage->GetActorLocation());
        if (Dist < NearestDist)
        {
            NearestDist = Dist;
            Nearest = Storage;
        }
    }

    return Nearest;
}

AResourceDepot* UTaskManagerComponent::FindNearestStorage(const AActor* Unit, const EResourceType NeededType) const
{
    if (!PC) return nullptr;
    if (!RPC || !RPC->RtsComponent) return nullptr;

    AResourceDepot* Nearest = nullptr;
    float NearestDist = FLT_MAX;
    const FVector MyLoc = Unit->GetActorLocation();

    for (AStructureBase* Build : RPC->RtsComponent->GetBuilds())
    {
        if (!Build || !Build->GetIsBuilt())
            continue;

        AResourceDepot* Storage = Cast<AResourceDepot>(Build);
        if (!Storage)
            continue;

        // 🔹 Récupère le stockage actuel et max pour ce type
        const int32 Current = Storage->GetStorage(NeededType);
        const int32 Max     = Storage->GetResourceMax(NeededType);

        // Si le stockage est plein -> skip
        if (Current >= Max)
            continue;

        // 🔹 Distance
        const float Dist = FVector::Dist(MyLoc, Storage->GetActorLocation());
        if (Dist < NearestDist)
        {
            NearestDist = Dist;
            Nearest = Storage;
        }
    }

    return Nearest;
}
