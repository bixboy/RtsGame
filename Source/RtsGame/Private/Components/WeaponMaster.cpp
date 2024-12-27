#include "Components/WeaponMaster.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

// Setup
#pragma region Setup

UWeaponMaster::UWeaponMaster()
{
	SetIsReplicatedByDefault(true);
}

void UWeaponMaster::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = Cast<APlayerController>(GetOwner()->GetInstigatorController());
}

void UWeaponMaster::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UWeaponMaster, HasAiming);
}

#pragma endregion

// Get Direction
FVector UWeaponMaster::GetDirection()
{
	if (!PlayerController) return FVector::ZeroVector;
	
	APlayerCameraManager* Camera = PlayerController->PlayerCameraManager;

	const FVector Start = Camera->GetCameraLocation();
	const FVector End = (UKismetMathLibrary::GetForwardVector(Camera->GetCameraRotation()) * WeaponRange) + Start;

	if (GetOwner()->HasAuthority())
	{
		return PerformSingleLineTrace(Start, End, ECollisionChannel::ECC_Visibility).TraceEnd;
	}
	else
	{
		return FVector::ZeroVector;
	}
}

// Shoot Trace
#pragma region Shoot Trace

TArray<FVector> UWeaponMaster::GetShootTrace(FVector Direction)
{
	StartEndPoints.Empty();

	if (GetOwner()->HasAuthority())
	{
		GetShootTrace_Server(Direction);
	}

	return StartEndPoints;
}

void UWeaponMaster::GetShootTrace_Server_Implementation(const FVector Direction)
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
			StartEndPoints = {HitResult.TraceStart, HitResult.ImpactPoint};
		}
		else
		{
			StartEndPoints = {HitResult.TraceStart, HitResult.TraceEnd};
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
			StartEndPoints = {HitResult.TraceStart, HitResult.ImpactPoint};
		}
		else
		{
			StartEndPoints = {HitResult.TraceStart, HitResult.TraceEnd};
		}
	}
}

#pragma endregion

// Spawn Bullet
#pragma region Spawn Bullet

void UWeaponMaster::SpawnBullet()
{
	if (!GetOwner()->HasAuthority())
	{
 		SpawnBullet_Server();
		return;
	}

	FVector SpawnLocation = GetSocketLocation("Ammo");
	FRotator SpawnRotation = UKismetMathLibrary::FindLookAtRotation(GetShootTrace(GetDirection())[0], GetShootTrace(GetDirection())[1]);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	GetWorld()->SpawnActor<AActor>(BulletClass, SpawnLocation, SpawnRotation, SpawnParams);
}

void UWeaponMaster::SpawnBullet_Server_Implementation()
{
	SpawnBullet();
}

#pragma endregion

// Set Aiming
void UWeaponMaster::SetHasAiming_Server_Implementation(bool Value)
{
	HasAiming = Value;
}

// Line Trace Function
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