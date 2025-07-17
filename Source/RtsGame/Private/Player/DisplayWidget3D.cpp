#include "Player/DisplayWidget3D.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"


ADisplayWidget3D::ADisplayWidget3D()
{
	PrimaryActorTick.bCanEverTick = true;

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
}

void ADisplayWidget3D::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADisplayWidget3D::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AlignWidgetToCamera();
}

void ADisplayWidget3D::AlignWidgetToCamera()
{
	FVector CameraLocation = FVector(0, 0, 0);
	FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CameraLocation);
	
	SetActorRotation(NewRotation);
}

