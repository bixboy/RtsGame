#include "Structures/DefenceStructure.h"
#include "Components/SphereComponent.h"


ADefenceStructure::ADefenceStructure()
{
	PrimaryActorTick.bCanEverTick = false;

	DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
	DetectionSphere->SetupAttachment(RootComponent);
	DetectionSphere->SetSphereRadius(1000.f);
	DetectionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}

void ADefenceStructure::BeginPlay()
{
	Super::BeginPlay();

    if (HasAuthority())
    {
        DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &ADefenceStructure::OnDetectionBegin);
        DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &ADefenceStructure::OnDetectionEnd);   
    }
}

void ADefenceStructure::OnDetectionBegin(UPrimitiveComponent* Overlapped, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Sweep)
{
    if (!Other || Other == this) return;
    
    if (Other->Implements<UFactionsInterface>() && IFactionsInterface::Execute_GetCurrentFaction(Other) != IFactionsInterface::Execute_GetCurrentFaction(this))
    {
        TargetsInRange.AddUnique(Other);
        StartAttacking();
    }
}

void ADefenceStructure::OnDetectionEnd(UPrimitiveComponent* Overlapped, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (!Other) return;
    
    TargetsInRange.Remove(Other);
    if (TargetsInRange.Num() == 0)
    {
        StopAttacking();
    }
}

void ADefenceStructure::StartAttacking()
{
    if (!GetWorld()->GetTimerManager().IsTimerActive(AttackTimerHandle))
    {
        GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, this, &ADefenceStructure::PerformAttack,AttackRate, true, 0.f);
    }
}

void ADefenceStructure::StopAttacking()
{
    GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
}

void ADefenceStructure::PerformAttack()
{
    if (TargetsInRange.Num() == 0) return;

    AActor* BestTarget = nullptr;
    float BestDistSq = FLT_MAX;
    const FVector MyLoc = GetActorLocation();

    for (AActor* Candidate : TargetsInRange)
    {
        if (!Candidate) continue;
        float DistSq = FVector::DistSquared(MyLoc, Candidate->GetActorLocation());
        if (DistSq < BestDistSq)
        {
            BestDistSq = DistSq;
            BestTarget = Candidate;
        }
    }

    if (!BestTarget) return;

    //Attack
    GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Emerald, BestTarget->GetName());
    DefenceAttack(BestTarget);
}

