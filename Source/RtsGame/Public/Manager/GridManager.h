#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridManager.generated.h"

class UGridComponent;


UCLASS()
class RTSGAME_API AGridManager : public AActor
{
	GENERATED_BODY()

public:
	AGridManager();

	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UGridComponent* GridComponent;
};
