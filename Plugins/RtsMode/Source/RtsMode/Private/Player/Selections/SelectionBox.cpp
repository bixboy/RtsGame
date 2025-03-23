#include "Player/Selections/SelectionBox.h"
#include "Player/PlayerControllerRts.h"
#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "Components/SlectionComponent.h"
#include "Interfaces/Selectable.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

ASelectionBox::ASelectionBox()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetBoxExtent(FVector(1.0f, 1.0f, 1.0f));
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Overlap);
	BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = BoxComponent;

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ASelectionBox::OnBoxCollisionBeginOverlap);

	Decal = CreateDefaultSubobject<UDecalComponent>(TEXT("Decal"));
	Decal->SetupAttachment(RootComponent);
	
	BoxSelect = false;
}

void ASelectionBox::BeginPlay()
{
	Super::BeginPlay();

	SetActorEnableCollision(false);
	if(Decal)
	{
		Decal->SetVisibility(false);
	}

	PlayerController = Cast<APlayerControllerRts>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
}

void ASelectionBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(BoxSelect)
	{
		Adjust();
		Manage();
	}
}

void ASelectionBox::Start(FVector Position, const FRotator Rotation)
{
	if(!Decal) return;

	StartLocation = FVector(Position.X, Position.Y, 0.0f);
	StartRotation = FRotator(0.f, Rotation.Yaw, 0.0f);

	SetActorLocation(StartLocation);
	SetActorRotation(StartRotation);
	SetActorEnableCollision(true);
	
	Decal->SetVisibility(true);
	InBox.Empty();
	CenterInBox.Empty();
	BoxSelect = true;
}

void ASelectionBox::End()
{
	if(!PlayerController) return;

	BoxSelect = false;
	SetActorEnableCollision(false);
	Decal->SetVisibility(false);

	if(CenterInBox.Num() == 0)
	{
		PlayerController->SelectionComponent->Handle_Selection(nullptr);	
	}
	else
	{
		PlayerController->SelectionComponent->Handle_Selection(CenterInBox);
	}

	GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Yellow, FString::Printf(TEXT("Current Selection: %i"), CenterInBox.Num()));

	InBox.Empty();
	CenterInBox.Empty();
}

void ASelectionBox::Adjust() const
{
	if (!PlayerController || !BoxComponent || !Decal) return;

	const FVector CurrentMouseLocOnTerrain = PlayerController->SelectionComponent->GetMousePositionOnTerrain().Location;
	const FVector EndPoint = FVector(CurrentMouseLocOnTerrain.X, CurrentMouseLocOnTerrain.Y, 0.0f);

	FVector NewLocation = UKismetMathLibrary::VLerp(StartLocation, EndPoint, 0.5f);
	BoxComponent->SetWorldLocation(NewLocation);

	FVector NewExtent = FVector(GetActorLocation().X, GetActorLocation().Y, 0.0f) - EndPoint;
	NewExtent = GetActorRotation().GetInverse().RotateVector(NewExtent);
	NewExtent = NewExtent.GetAbs();
	NewExtent.Z += 100000.f;

	BoxComponent->SetBoxExtent(NewExtent);

	FVector DecalSize = FVector(NewExtent.Z, NewExtent.Y, NewExtent.X);
	Decal->DecalSize = DecalSize;
}

void ASelectionBox::Manage()
{
    if (!BoxComponent) return;

    const FVector BoxExtent = BoxComponent->GetScaledBoxExtent();
    TArray<AActor*> ValidCandidates;

    // Récupérer les acteurs présents dans la box
    for (AActor* Actor : InBox)
    {
        if (!Actor) continue;
        
        FVector ActorCenter = Actor->GetActorLocation();
        FVector LocalActorCenter = BoxComponent->GetComponentTransform().InverseTransformPosition(ActorCenter);
    	
        bool bInsideBox = FMath::Abs(LocalActorCenter.X) <= BoxExtent.X &&
                          FMath::Abs(LocalActorCenter.Y) <= BoxExtent.Y;
        
        if (bInsideBox)
        {
            ValidCandidates.Add(Actor);
        }
        else
        {
            CenterInBox.Remove(Actor);
            HandleHighlight(Actor, false);
        }
    }
    
    if (ValidCandidates.Num() > 0)
    {
        auto FirstType = ISelectable::Execute_GetSelectionType(ValidCandidates[0]);
        
        TArray<AActor*> FilteredCandidates;
        for (AActor* Actor : ValidCandidates)
        {
            if (ISelectable::Execute_GetSelectionType(Actor) == FirstType)
            {
                FilteredCandidates.Add(Actor);
            }
            else
            {
                HandleHighlight(Actor, false);
            }
        }
    	
        for (AActor* Actor : FilteredCandidates)
        {
            if (!CenterInBox.Contains(Actor))
            {
                CenterInBox.Add(Actor);
                HandleHighlight(Actor, true);
            }
        }
    }
    else
    {
        for (AActor* Actor : CenterInBox)
        {
            HandleHighlight(Actor, false);
        }
        CenterInBox.Empty();
    }
}

void ASelectionBox::HandleHighlight(AActor* ActorInBox, const bool Highlight) const
{
	if(ISelectable* Selectable = Cast<ISelectable>(ActorInBox))
	{
		Selectable->Highlight(Highlight);
	}
}

void ASelectionBox::OnBoxCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!OtherActor || OtherActor == this) return;

	if(ISelectable* Selectable = Cast<ISelectable>(OtherActor))
	{
		InBox.AddUnique(OtherActor);
	}
}

