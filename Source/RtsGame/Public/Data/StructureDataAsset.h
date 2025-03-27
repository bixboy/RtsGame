#pragma once
#include "CoreMinimal.h"
#include "DataRts.h"
#include "Engine/DataAsset.h"
#include "StructureDataAsset.generated.h"


UCLASS()
class UStructureDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FStructure Structure;
};
