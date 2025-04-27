#pragma once
#include "CoreMinimal.h"
#include "Player/Selections/SelectionBox.h"
#include "RtsSelectionBox.generated.h"

class ARtsPlayerController;


UCLASS()
class RTSGAME_API ARtsSelectionBox : public ASelectionBox
{
	GENERATED_BODY()

public:
	ARtsSelectionBox();

protected:
	virtual void BeginPlay() override;

	virtual void OnBoxCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UPROPERTY()
	ARtsPlayerController* RtsController;
};

