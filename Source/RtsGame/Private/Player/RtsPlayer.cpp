#include "Player/RtsPlayer.h"
#include "Components/RtsComponent.h"
#include "Player/RtsPlayerController.h"
#include "Structures/StructurePreview.h"


// --------------- Setup ---------------
#pragma region Setup

ARtsPlayer::ARtsPlayer()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ARtsPlayer::BeginPlay()
{
	Super::BeginPlay();

	RtsController = Cast<ARtsPlayerController>(GetController());
	CreatePreviewMesh();
}

void ARtsPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARtsPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

#pragma endregion


void ARtsPlayer::CreatePreviewMesh()
{
	if(PreviewUnits) return;

	if (RtsController && RtsController->IsLocalController())
	{
		if(UWorld* World = GetWorld())
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Instigator = this;
			SpawnParams.Owner = this;
		
			Preview = World->SpawnActor<AStructurePreview>(BuildPreviewClass,FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
			PreviewUnits = Preview;
			
			if (Preview)
			{
				Preview->SetReplicates(false);
				Preview->SetOwner(this);
			
				RtsController->RtsComponent->OnUnitUpdated.RemoveDynamic(this, &ARtsPlayer::ShowUnitPreview);
				RtsController->RtsComponent->OnUnitUpdated.AddDynamic(this, &ARtsPlayer::ShowUnitPreview);

				RtsController->RtsComponent->OnBuildUpdated.RemoveDynamic(this, &ARtsPlayer::ShowBuildPreview);
				RtsController->RtsComponent->OnBuildUpdated.AddDynamic(this, &ARtsPlayer::ShowBuildPreview);
			}
		}
	}
}

void ARtsPlayer::ShowBuildPreview(const FStructure BuildData)
{
	if (Preview && BuildData.StructureMesh)
	{
		bIsInSpawnUnits = true;
		bIsInSpawnBuild = true;
		bPreviewFollowMouse = true;
		Preview->EnabledCollision(true);
		
		Preview->StartPlacingBuilding(BuildData);
		Preview->SetActorLocation(RtsController->SelectionComponent->GetMousePositionOnTerrain().Location);
	}
}

void ARtsPlayer::ShowUnitPreview(TSubclassOf<ASoldierRts> NewUnitClass)
{
	bIsInSpawnBuild = false;
	Preview->EnabledCollision(false);
	
	Super::ShowUnitPreview(NewUnitClass);
}

void ARtsPlayer::HidePreview()
{
	bIsInSpawnBuild = false;
	RtsController->RtsComponent->ClearPreviewClass();
	
	Super::HidePreview();
}

void ARtsPlayer::Input_OnSpawnUnits()
{
	if (!bIsInSpawnBuild || !Preview)
	{
		Super::Input_OnSpawnUnits();
		return;
	}

	if (bIsInSpawnBuild && Preview)
	{
		if (Preview->GetIsValidPlacement())
		{
			RtsController->RtsComponent->SpawnBuild();	
		}
	}
}


