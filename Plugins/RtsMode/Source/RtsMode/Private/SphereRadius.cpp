#include "SphereRadius.h"
#include "PlayerControllerRts.h"
#include "Components/DecalComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

ASphereRadius::ASphereRadius()
{
	PrimaryActorTick.bCanEverTick = true;

	
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetSphereRadius(1.f);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Overlap);
	SphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = SphereComponent;
	
	Decal = CreateDefaultSubobject<UDecalComponent>(TEXT("Decal"));
	Decal->SetupAttachment(RootComponent);
	
	SphereEnable = false;
}

void ASphereRadius::BeginPlay()
{
	Super::BeginPlay();
	
	if(Decal) Decal->SetVisibility(false);
	
	SetActorEnableCollision(false);
	PlayerController = Cast<APlayerControllerRts>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
}

void ASphereRadius::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if(SphereEnable)
		CurrentRadius = Adjust();
}

float ASphereRadius::Adjust() const
{
	if (!PlayerController || !SphereComponent || !Decal) return 0.f;

	const FVector CurrentMouseLocOnTerrain = PlayerController->GetMousePositionOnTerrain();
	const FVector EndPoint = FVector(CurrentMouseLocOnTerrain.X, CurrentMouseLocOnTerrain.Y, 0.0f);

	FVector NewLocation = UKismetMathLibrary::VLerp(StartLocation, EndPoint, 0.5f);
	SphereComponent->SetWorldLocation(NewLocation);

	float NewScale = FVector::Distance(FVector(NewLocation.X, NewLocation.Y, 0.0f), EndPoint);
	//SphereComponent->SetSphereRadius(NewScale);

	FVector DecalSize = FVector(NewScale * 2.0f, NewScale * 2.0f, NewScale * 2.0f);
	Decal->DecalSize = DecalSize;

	return NewScale;
}

void ASphereRadius::Start(FVector Position, const FRotator Rotation)
{
	if(!Decal) return;

	StartLocation = FVector(Position.X, Position.Y, 0.0f);
	StartRotation = FRotator(0.f, Rotation.Yaw, 0.0f);

	SetActorLocation(StartLocation);
	SetActorRotation(StartRotation);
	SetActorEnableCollision(true);
	
	Decal->SetVisibility(true);
	SphereEnable = true;
}

void ASphereRadius::End()
{
	if(!Decal) return;

	SphereEnable = false;
	SetActorEnableCollision(false);
	Decal->SetVisibility(false);
}

float ASphereRadius::GetRadius() const
{
	return CurrentRadius;
}

