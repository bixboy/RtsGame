#include "Components/Weapons/ArtilleryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapons/Projectile.h"


UArtilleryComponent::UArtilleryComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UArtilleryComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UArtilleryComponent::TickComponent(float Delta, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(Delta, TickType, ThisTickFunction);
}

void UArtilleryComponent::StartAIFiring(AActor* TargetActor)
{
    if (!TargetActor || !GetOwner() || !GetOwner()->HasAuthority()) return;
    
    AITarget = TargetActor;
    HandleAIFire();
}

void UArtilleryComponent::StopAIFiring()
{
    AITarget = nullptr;
    GetWorld()->GetTimerManager().ClearTimer(TimerHandle_AI);
}

float UArtilleryComponent::GetArcParamForDistance(float Distance) const
{
    float Alpha = 0;
    return Alpha;
}

void UArtilleryComponent::HandleAIFire()
{
    if (!bCanFire || !AITarget || !ProjectileClass) return;

    FVector MuzzleLoc;
    if (auto* Mesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>())
    {
        MuzzleLoc = Mesh->GetSocketLocation(MuzzleSocketName);
    }
    else
    {
        MuzzleLoc = GetOwner()->GetActorLocation();
    }

    FVector TargetLoc = AITarget->GetActorLocation();

    float SpreadRadius = ShotSpread;
    FVector2D RandomOffset2D = FMath::RandPointInCircle(SpreadRadius);
    TargetLoc += FVector(RandomOffset2D.X, RandomOffset2D.Y, 0.f);
    
    float Distance2D = FVector::Dist2D(MuzzleLoc, TargetLoc);
    
    float ArcParam = FMath::GetMappedRangeValueClamped(
        FVector2D(300.f, 3000.f),
        FVector2D(1.f, 0.3f),
        Distance2D
    );
    

    FVector LaunchVelocity;
    bool bFound = UGameplayStatics::SuggestProjectileVelocity_CustomArc(
        GetWorld(),
        LaunchVelocity,
        MuzzleLoc,
        TargetLoc,
        0.f,
        ArcParam
    );

    if (!bFound)
    {
        return;
    }

    // Debug
    {
        DrawDebugLine(GetWorld(), MuzzleLoc, MuzzleLoc + LaunchVelocity.GetSafeNormal() * 100.f, FColor::Blue, false, 2.f, 0, 2.f);

        FVector SimulatedLoc = MuzzleLoc;
        FVector SimulatedVel = LaunchVelocity;
        const float TimeStep = 0.1f;
        const float GravityZ = GetWorld()->GetGravityZ();
        for (float Time = 0.f; Time < 5.f; Time += TimeStep)
        {
            FVector NextLoc = SimulatedLoc + SimulatedVel * TimeStep;
            SimulatedVel.Z += GravityZ * TimeStep;
            DrawDebugLine(GetWorld(), SimulatedLoc, NextLoc, FColor::Yellow, false, 2.f, 0, 1.f);
            SimulatedLoc = NextLoc;
        }
        DrawDebugSphere(GetWorld(), SimulatedLoc, 20.f, 8, FColor::Green, false, 2.f);   
    }
    
    Server_Fire(MuzzleLoc, LaunchVelocity.Rotation(), LaunchVelocity);
    bCanFire = true;
}

void UArtilleryComponent::Server_Fire_Implementation(const FVector MuzzleLoc, const FRotator MuzzleRot, FVector LaunchSpeed)
{
    FActorSpawnParameters Params;
    Params.Owner = GetOwner();
    
    AProjectile* Proj = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, MuzzleLoc, MuzzleRot, Params);
    if (Proj)
    {
        Proj->LaunchProjectile(LaunchSpeed);
    }

    // Effets
    Multicast_FireEffects(MuzzleLoc, MuzzleRot);
    OnArtilleryFired.Broadcast();
}

void UArtilleryComponent::Multicast_FireEffects_Implementation(const FVector& MuzzleLoc, const FRotator& MuzzleRot)
{
    if (MuzzleFlashFX)
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlashFX, MuzzleLoc, MuzzleRot);
    if (FireSound)
        UGameplayStatics::PlaySoundAtLocation(this, FireSound, MuzzleLoc);
}

