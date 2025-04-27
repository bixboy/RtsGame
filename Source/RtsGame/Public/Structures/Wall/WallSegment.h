#pragma once
#include "CoreMinimal.h"
#include "Structures/StructureBase.h"
#include "WallSegment.generated.h"


UCLASS()
class RTSGAME_API AWallSegment : public AStructureBase
{
	GENERATED_BODY()

public:
	AWallSegment();

	UFUNCTION()
	FTransform GetFrontTransform() const { return SnapPointFront->GetComponentTransform(); }

	UFUNCTION()
	FTransform GetBackTransform() const { return SnapPointBack->GetComponentTransform(); }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* SnapPointFront;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* SnapPointBack;

};
