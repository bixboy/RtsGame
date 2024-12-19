#include "SelectionBox.h"

#include "PlayerControllerRts.h"
#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
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

	PlayerController = Cast<APlayerControllerRts>(UGameplayStatics::GetPlayerController(GetWorld(), 0))	;
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
		PlayerController->Handle_Selection(nullptr);	
	}
	else
	{
		PlayerController->Handle_Selection(CenterInBox);
	}

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Current Selection: %i"), CenterInBox.Num()));

	InBox.Empty();
	CenterInBox.Empty();
}

void ASelectionBox::Adjust() const
{
	if (!PlayerController || !BoxComponent || !Decal) return;

	const FVector CurrentMouseLocOnTerrain = PlayerController->GetMousePositionOnTerrain();
	const FVector EndPoint = FVector(CurrentMouseLocOnTerrain.X, CurrentMouseLocOnTerrain.Y, 0.0f);

	FVector NewLocation = UKismetMathLibrary::VLerp(StartLocation, EndPoint, 0.5f);
	BoxComponent->SetWorldLocation(NewLocation);

	FVector NewExtent = FVector(GetActorLocation().X, GetActorLocation().Y, 0.0f) - EndPoint;
	NewExtent = GetActorRotation().GetInverse().RotateVector(NewExtent);
	NewExtent = NewExtent.GetAbs();
	NewExtent.Z += 100000.f;

	BoxComponent->SetBoxExtent(NewExtent);

	FVector DecalSize = FVector(NewExtent.X, NewExtent.Y, NewExtent.X);
	Decal->DecalSize = DecalSize;

}

void ASelectionBox::Manage()
{
	if (!BoxComponent)
	{
		return;
	}

	for (int i = 0; i < InBox.Num(); ++i)
	{
		// get actor center and transform into the local space of the collider
		FVector ActorCenter = InBox[i]->GetActorLocation();
		const FVector LocalActorCenter = BoxComponent->GetComponentTransform().InverseTransformPosition(ActorCenter);

		DrawDebugSphere(GetWorld(), ActorCenter, 50.f, 8, FColor::Blue, false, -1, 0, 5.f);

		//Get the local extents of the collider
		const FVector LocalExtents = BoxComponent->GetUnscaledBoxExtent();
		if (LocalActorCenter.X >= -LocalExtents.X && LocalActorCenter.X <= LocalExtents.X
			&& LocalActorCenter.Y >= -LocalExtents.Y && LocalActorCenter.Y <= LocalExtents.Y
			&& LocalActorCenter.Z >= -LocalExtents.Z && LocalActorCenter.Z <= LocalExtents.Z)
		{

			// add object to CenterBox is in selection box list
			if (!CenterInBox.Contains(InBox[i]))
			{
				CenterInBox.Add(InBox[i]);
				HandleHighlight(InBox[i], true);
			}

		}
		else
		{
			CenterInBox.Remove(InBox[i]);
			HandleHighlight(InBox[i], false);
		}
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

