#pragma once

#include "CoreMinimal.h"
#include "WalkerVehicles.h"
#include "Walker_ATAP.generated.h"

UCLASS()
class LANDVEHICULE_API AWalker_ATAP : public AWalkerVehicles
{
	GENERATED_BODY()

public:
	AWalker_ATAP();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

};
