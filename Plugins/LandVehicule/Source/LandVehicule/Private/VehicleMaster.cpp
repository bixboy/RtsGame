#include "VehicleMaster.h"

AVehicleMaster::AVehicleMaster()
{
	PrimaryActorTick.bCanEverTick = true;

	BaseVehicle = CreateDefaultSubobject<UStaticMeshComponent>("BaseVehicle");
	RootComponent = BaseVehicle;
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(BaseVehicle);
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	SpringArm->TargetArmLength = CameraDistance;
	SpringArm->TargetOffset = FVector(0, 0, 200.f);
	Camera->bUsePawnControlRotation = true;
}

void AVehicleMaster::BeginPlay()
{
	Super::BeginPlay();
	
}

void AVehicleMaster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Velocity = BaseVehicle->GetPhysicsLinearVelocity();
	float Speed = Velocity.Size();

	if (Speed > MaxSpeed)
	{
		Velocity = Velocity.GetClampedToMaxSize(MaxSpeed);
		BaseVehicle->SetPhysicsLinearVelocity(Velocity);
	}
}

void AVehicleMaster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AVehicleMaster::TurnEngineOn(bool OnOff)
{
	EngineOn = OnOff;
	PlaySond(SoundEngineOn);
}

void AVehicleMaster::MoveForward(float Value)
{
	ForwardInput = Value;
}

void AVehicleMaster::MoveRight(float Value)
{
	TurnInput = Value;
}

float AVehicleMaster::GetForwardInput() const
{
	return ForwardInput;
}

float AVehicleMaster::GetTurnInput() const
{
	return TurnInput;
}

