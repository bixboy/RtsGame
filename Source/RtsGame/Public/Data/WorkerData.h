#pragma once

#include "CoreMinimal.h"
#include "DataRts.h"
#include "WorkerData.generated.h"


UENUM(BlueprintType)
enum class ETaskType : uint8
{
	None,
	Collect,
	Build,
	Deliver,
	Attack
};

USTRUCT(BlueprintType)
struct FTaskJob
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName JobName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bForced = false;
	
	// ==== Job
	UPROPERTY()
	int32 JobId = -1;

	UPROPERTY()
	ETaskType TaskType = ETaskType::None;

	UPROPERTY()
	float Priority = 0.f;

	UPROPERTY()
	int32 RequiredWorkers = 1;
	
	// ==== Target
	UPROPERTY()
	AActor* Target = nullptr;

	UPROPERTY()
	FResourcesCost ResourcesNeeded;
	
	// ==== Other
	ECommandType MovementType = CommandMove;

	UPROPERTY()
	APlayerController* Owner = nullptr;
};
