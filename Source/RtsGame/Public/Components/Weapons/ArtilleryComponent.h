#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ArtilleryComponent.generated.h"

class AProjectile;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnArtilleryFired);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RTSGAME_API UArtilleryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
    UArtilleryComponent();
    
    UFUNCTION(BlueprintCallable, Category="Artillery|AI")
    void StartAIFiring(AActor* TargetActor);

   
    UFUNCTION(BlueprintCallable, Category="Artillery|AI")
    void StopAIFiring();

    
    UPROPERTY(BlueprintAssignable, Category="Artillery")
    FOnArtilleryFired OnArtilleryFired;

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float Delta, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // Properties...
    UPROPERTY(EditAnywhere, Category="Artillery")
    TSubclassOf<AProjectile> ProjectileClass;

    UPROPERTY(EditAnywhere, Category="Artillery")
    FName MuzzleSocketName = TEXT("Muzzle");

    UPROPERTY(EditAnywhere, Category = "Artillery")
    float ShotSpread = 0.f;

    // FX & Son
    UPROPERTY(EditAnywhere, Category="Artillery")
    UParticleSystem* MuzzleFlashFX;

    UPROPERTY(EditAnywhere, Category="Artillery")
    USoundBase* FireSound;

private:
    // État tir
    UPROPERTY()
    bool bCanFire = true;
    
    UPROPERTY()
    FTimerHandle TimerHandle_Cooldown;

    // IA
    UPROPERTY()
    AActor* AITarget = nullptr;
    
    UPROPERTY()
    FTimerHandle TimerHandle_AI;
    
    UFUNCTION()
    void HandleAIFire();

    UFUNCTION()
    float GetArcParamForDistance(float Distance) const;

    UFUNCTION(Server, Reliable)
    void Server_Fire(const FVector MuzzleLoc, const FRotator MuzzleRot, FVector LaunchSpeed);

    UFUNCTION(NetMulticast, Unreliable)
    void Multicast_FireEffects(const FVector& MuzzleLoc, const FRotator& MuzzleRot);
    
};
