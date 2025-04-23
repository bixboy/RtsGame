#include "Weapons/Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"


AProjectile::AProjectile()
{
    PrimaryActorTick.bCanEverTick = false;
    
    // Collision setup for overlap
    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
    CollisionComponent->InitSphereRadius(10.f);
    CollisionComponent->SetCollisionProfileName("OverlapAllDynamic");
    CollisionComponent->SetGenerateOverlapEvents(true);
    RootComponent = CollisionComponent;

    // Mesh
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    MeshComponent->SetupAttachment(RootComponent);
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // Movement
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
    ProjectileMovement->InitialSpeed = 0.f;
    ProjectileMovement->MaxSpeed = 0.f;
    
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bInitialVelocityInLocalSpace = false;
    
    ProjectileMovement->ProjectileGravityScale = 1.f;
    InitialLifeSpan = LifeSpan;
}

void AProjectile::BeginPlay()
{
    Super::BeginPlay();
    
    if (HasAuthority() && CollisionComponent)
    {
        CollisionComponent->OnComponentBeginOverlap.RemoveDynamic(this, &AProjectile::OnOverlapBegin);
        CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnOverlapBegin);
    }
}

void AProjectile::LaunchProjectile(FVector InVelocity)
{
    if (ProjectileMovement)
    {
        ProjectileMovement->Velocity = InVelocity;
        ProjectileMovement->Activate();
    }
}

void AProjectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!HasAuthority() || OtherActor == this || OtherActor == GetOwner())
    {
        return;
    }

    // Spawn effect
    if (ImpactEffect)
    {
        UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(), ImpactEffect, SweepResult.ImpactPoint, SweepResult.ImpactNormal.Rotation());
    }

    // Play sound
    if (ImpactSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, SweepResult.ImpactPoint);
    }

    // Apply damage
    if (OtherActor && OtherActor != this)
    {
        UGameplayStatics::ApplyDamage(OtherActor, Damage, GetInstigatorController(), this, nullptr);
    }

    if (bDestroyOnImpact)
    {
        Destroy();
    }
}
