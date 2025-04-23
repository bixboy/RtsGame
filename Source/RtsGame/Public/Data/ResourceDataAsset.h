#pragma once
#include "CoreMinimal.h"
#include "DataRts.h"
#include "Engine/DataAsset.h"
#include "ResourceDataAsset.generated.h"


UCLASS()
class RTSGAME_API UResourceDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FResourcesCost Resources;
};
