#include "Structures/UnitsProduction/UnitProduction.h"
#include "Components/DecalComponent.h"
#include "Data/UnitsProductionDataAsset.h"
#include "Net/UnrealNetwork.h"
#include "Units/UnitsMaster.h"


// ----------------- Setup -----------------
#pragma region Setup

AUnitProduction::AUnitProduction()
{
	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates = true;

	spawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("spawnPoint"));
	spawnPoint->SetupAttachment(RootComponent);
	
	DestPoint = CreateDefaultSubobject<USceneComponent>(TEXT("DestPoint"));
	DestPoint->SetupAttachment(RootComponent);

	DecalCursor = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalCursor"));
	DecalCursor->SetupAttachment(DestPoint);

	OnSelected.AddDynamic(this, &AUnitProduction::OnNewSelected);
}

void AUnitProduction::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
		SetOwner(PC);

	OnBuildComplete.AddDynamic(this, &AUnitProduction::StartUnitProduction);
}

void AUnitProduction::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AUnitProduction, UnitSelected);
	DOREPLIFETIME(AUnitProduction, ProductionQueue);
	DOREPLIFETIME(AUnitProduction, ProductionProgress);
}

#pragma endregion


void AUnitProduction::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (GetWorld()->GetTimerManager().IsTimerActive(ProductionTimerHandle) && UnitSelected)
	{
		float ElapsedTime = GetWorld()->GetTimeSeconds() - ProductionStartTime;
		float TotalTime = UnitSelected->UnitProduction.ProductionTime;
		ProductionProgress = FMath::Clamp(ElapsedTime / TotalTime, 0.f, 1.f);
	}
	else
	{
		ProductionProgress = 0.f;
	}
}

void AUnitProduction::OnNewSelected(bool bIsSelected)
{
	DecalCursor->SetHiddenInGame(!bIsSelected);
}


// ----------------- Queue Production -----------------

void AUnitProduction::AddUnitToProduction_Implementation(UUnitsProductionDataAsset* NewUnit)
{
	Server_AddUnitToQueue(NewUnit);
}

void AUnitProduction::Server_AddUnitToQueue_Implementation(UUnitsProductionDataAsset* NewUnit)
{
	if(NewUnit)
	{
		ProductionQueue.Add(NewUnit);
        
		if(GetIsBuilt() && !GetWorld()->GetTimerManager().IsTimerActive(ProductionTimerHandle))
		{
			StartUnitProduction();
		}
	}
}

void AUnitProduction::RemoveUnitFromQueue(UUnitsProductionDataAsset* UnitToRemove)
{
	if (!UnitToRemove || !HasAuthority()) return;

	const int32 Index = ProductionQueue.IndexOfByKey(UnitToRemove);
	if (Index != INDEX_NONE)
	{
		const bool bIsCurrent = (Index == 0);

		ProductionQueue.RemoveAt(Index);
		
		if (bIsCurrent)
		{
			StopUnitProduction();

			if (GetIsBuilt() && ProductionQueue.Num() > 0)
			{
				StartUnitProduction();
			}
			else
			{
				UnitSelected = nullptr;
			}
		}
	}
}


// ----------------- Production -----------------

void AUnitProduction::StartUnitProduction()
{
	if (HasAuthority())
	{
		if (!ProductionQueue.IsEmpty())
		{
			GEngine->AddOnScreenDebugMessage(-1, 7.0f, FColor::Red, "Starting UnitProduction");
			UnitSelected = ProductionQueue[0];
			ProductionStartTime = GetWorld()->GetTimeSeconds();
			ProductionProgress = 0.f;
			
			GetWorld()->GetTimerManager().SetTimer(ProductionTimerHandle, this, &AUnitProduction::OnProductionFinished, UnitSelected->UnitProduction.ProductionTime, false);
		}
	}
	else Server_StartUnitProduction();
}

void AUnitProduction::StopUnitProduction()
{
	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().ClearTimer(ProductionTimerHandle);
	}
	else Server_StopUnitProduction();
}

void AUnitProduction::OnProductionFinished()
{
	if (UnitSelected && UnitSelected->UnitProduction.UnitClass)
    {
        FVector SpawnLocation = spawnPoint->GetComponentLocation();
        FRotator SpawnRotation = GetActorRotation();
	    	
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
	    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;	

        if (AUnitsMaster* SpawnedUnit = GetWorld()->SpawnActor<AUnitsMaster>(UnitSelected->UnitProduction.UnitClass, SpawnLocation, SpawnRotation, SpawnParams))
        {
            OnUnitProduced.Broadcast(SpawnedUnit);

        	SpawnedUnit->UnitInfo = UnitSelected;
        	
            FCommandData CommandData = GetDestination();
            CommandData.Type = ECommandType::CommandMove;
                
            ISelectable::Execute_CommandMove(SpawnedUnit, CommandData);
        }
    }

    ProductionStartTime = GetWorld()->GetTimeSeconds();
    ProductionProgress = 0.f;

    if (ProductionQueue.Num() > 0)
        ProductionQueue.RemoveAt(0);

    if (ProductionQueue.Num() > 0)
    {
        UnitSelected = ProductionQueue[0];
        GetWorld()->GetTimerManager().SetTimer(ProductionTimerHandle, this, &AUnitProduction::OnProductionFinished, UnitSelected->UnitProduction.ProductionTime, false);
    }
}

FCommandData AUnitProduction::GetDestination()
{
	FVector CurrentDest = DestPoint->GetComponentLocation();
	const float Tolerance = 1.f;

	FCommandData CommandData;

	if ( (LastDestination - CurrentDest).SizeSquared() < FMath::Square(Tolerance) )
	{
		FormationUnitCount++;

		int32 NumPerRow = FMath::CeilToInt(FMath::Sqrt(static_cast<double>(FormationUnitCount)));
		int32 Row = (FormationUnitCount - 1) / NumPerRow;
		int32 Col = (FormationUnitCount - 1) % NumPerRow;

		float Spacing = 250.f;

		FVector FormationOffset = FVector(Row * Spacing, Col * Spacing, 0.f);
		CommandData.Location = CurrentDest + FormationOffset;
	}
	else
	{
		LastDestination = CurrentDest;
		FormationUnitCount = 1;
		CommandData.Location = CurrentDest;
	}

	return CommandData;
}

float AUnitProduction::GetProductionProgress()
{
	return ProductionProgress;
}


#pragma region Replcation

void AUnitProduction::Server_StartUnitProduction_Implementation()
{
	StartUnitProduction();
}

void AUnitProduction::Server_StopUnitProduction_Implementation()
{
	StopUnitProduction();
}

void AUnitProduction::Multicast_OnUnitProduced_Implementation(AActor* NewUnit)
{
	OnUnitProduced.Broadcast(NewUnit);
}

#pragma endregion

