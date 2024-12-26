#include "Components/WeaponMaster.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

FVector UWeaponMaster::GetDirection()
{
	APlayerCameraManager* Camera = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);

	const FVector Start = Camera->GetCameraLocation();
	const FVector End = (UKismetMathLibrary::GetForwardVector(Camera->GetCameraRotation()) * WeaponRange) + Start;
	
	return PerformSingleLineTrace(Start, End, ECollisionChannel::ECC_Visibility).TraceEnd;
}

TArray<FVector> UWeaponMaster::GetShootTrace(FVector Direction)
{
	FVector TraceDirection = Direction;
	const FVector Start = GetSocketLocation("Ammo");

	if (HasAiming)
	{
		const FVector End = (((TraceDirection - Start) / (TraceDirection - Start).Length()) * 15000.f + TraceDirection)
		+ FVector(
			UKismetMathLibrary::RandomFloatInRange(-70.f, 70.f), // Vector X
			UKismetMathLibrary::RandomFloatInRange(-70.f, 70.f), // Vector Y
			UKismetMathLibrary::RandomFloatInRange(-70.f, 70.f) // Vector Z
			);
		
		FHitResult HitResult = PerformSingleLineTrace(Start, End, ECollisionChannel::ECC_Visibility);
		if (HitResult.GetActor())
		{
			return{HitResult.TraceStart, HitResult.ImpactPoint};
		}
		else
		{
			return{HitResult.TraceStart, HitResult.TraceEnd};
		}
	}
	else
	{
		const FVector End = (((TraceDirection - Start) / (TraceDirection - Start).Length()) * 15000.f + TraceDirection)
		+ FVector(
			UKismetMathLibrary::RandomFloatInRange(-750.f, 750.f), // Vector X
			UKismetMathLibrary::RandomFloatInRange(-750.f, 750.f), // Vector Y
			UKismetMathLibrary::RandomFloatInRange(-750.f, 750.f) // Vector Z
			);
		
		FHitResult HitResult = PerformSingleLineTrace(Start, End, ECollisionChannel::ECC_Visibility);
		if (HitResult.GetActor())
		{
			return{HitResult.TraceStart, HitResult.ImpactPoint};
		}
		else
		{
			return{HitResult.TraceStart, HitResult.TraceEnd};
		}
	}
}

FHitResult UWeaponMaster::PerformSingleLineTrace(const FVector& Start, const FVector& End, ECollisionChannel TraceChannel) const
{
	FHitResult HitResult;
	
	FCollisionQueryParams TraceParams;
	TraceParams.bTraceComplex = true;
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.AddIgnoredActor(GetOwner());

	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, TraceChannel, TraceParams);
	return HitResult;
}

void UWeaponMaster::SpawnBullet()
{
	FVector SpawnLocation = GetSocketLocation("Ammo");
	FRotator SpawnRotation = UKismetMathLibrary::FindLookAtRotation(GetShootTrace(GetDirection())[0], GetShootTrace(GetDirection())[1]);
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	GetWorld()->SpawnActor<AActor>(BulletClass, SpawnLocation, SpawnRotation, SpawnParams);
}
