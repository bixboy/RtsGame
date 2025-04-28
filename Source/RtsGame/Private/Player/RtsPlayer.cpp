#include "Player/RtsPlayer.h"
#include "Components/RtsComponent.h"
#include "Player/RtsPlayerController.h"
#include "Structures/Preview/StructurePreview.h"
#include "Structures/Wall/WallSegment.h"


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

	if (RtsController)
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


void ARtsPlayer::Input_SelectAllUnitType()
{
	const TArray<AActor*>& Selected = Player->SelectionComponent->GetSelectedActors();
	if (Selected.Num() == 0) return;

	AActor* Reference = Selected[0];
	if (!Reference) return;

	UClass* TargetClass = Reference->GetClass();

	TArray<AActor*> Candidates = GetAllActorsOfClassInCameraBound<AActor>(GetWorld(), TargetClass);
	if (Candidates.Num() == 0) return;

	const bool bReferenceHasFaction = Reference->Implements<UFactionsInterface>();
	const EFaction ReferenceFaction = bReferenceHasFaction ? IFactionsInterface::Execute_GetCurrentFaction(Reference) : EFaction::None;

	TArray<AActor*> ActorsToSelect;
	ActorsToSelect.Reserve(Candidates.Num());

	for (AActor* Candidate : Candidates)
	{
		if (!Candidate)
		{
			continue;
		}

		if (bReferenceHasFaction && Candidate->Implements<UFactionsInterface>())
		{
			EFaction CandidateFaction = IFactionsInterface::Execute_GetCurrentFaction(Candidate);
			if (CandidateFaction != ReferenceFaction)
			{
				continue;
			}
		}

		ActorsToSelect.Add(Candidate);
	}

	if (ActorsToSelect.Num() > 0)
	{
		Player->SelectionComponent->Handle_Selection(ActorsToSelect);
	}
}


// ========== Preview ========== //
void ARtsPlayer::CreatePreviewMesh()
{
	if(PreviewUnits) return;
	
	if (RtsController)
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
		bIsInSpawnWall = false;
		
		Preview->EnabledCollision(true);
		Preview->SetActorLocation(RtsController->SelectionComponent->GetMousePositionOnTerrain().Location);

		if (BuildData.BuildClass.Get()->IsChildOf(AWallSegment::StaticClass()))
		{
			bIsInSpawnWall = true;
			Preview->StartWallPreview(BuildData);
		}
		else
		{
			Preview->StartPlacingBuilding(BuildData);
		}
	}
}

void ARtsPlayer::ShowUnitPreview(TSubclassOf<ASoldierRts> NewUnitClass)
{
	bIsInSpawnBuild = false;
	bIsInSpawnWall = false;
	Preview->EnabledCollision(false);
	
	Super::ShowUnitPreview(NewUnitClass);
}

void ARtsPlayer::HidePreview()
{
	bIsInSpawnBuild = false;
	bIsInSpawnWall = false;
	RtsController->RtsComponent->ClearPreviewClass();

	Preview->StopWallPreview();
	
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
		if (bIsInSpawnWall)
		{
			if (!bPreviewFollowMouse)
			{
				TArray<FTransform> WallsTransform = Preview->ConfirmWallPreview();
				if (WallsTransform.IsEmpty())
					return;
				
				for (FTransform WallTransform : WallsTransform)
				{
					RtsController->RtsComponent->SpawnBuild(WallTransform);	
				}

				Preview->StopWallPreview();
				bPreviewFollowMouse = true;
				
				return;
			}

			bPreviewFollowMouse = false;
			Preview->ShowWallPreview(RtsController->RtsComponent->GetMousePositionOnTerrain().Location);
			return;
		}
		
		if (Preview->GetIsValidPlacement())
		{
			RtsController->RtsComponent->SpawnBuild();	
		}
	}
}


