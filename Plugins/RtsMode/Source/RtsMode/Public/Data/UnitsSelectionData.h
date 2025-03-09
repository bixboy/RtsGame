#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "UnitsSelectionData.generated.h"
 
class ASoldierRts;

USTRUCT(BlueprintType)
struct FUnitsSelectionData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit")
	UTexture2D* UnitImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit")
	FText UnitName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit")
	TSubclassOf<ASoldierRts> UnitClass;
};
