#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "VehiclesInteractions.generated.h"

UINTERFACE()
class UVehiclesInteractions : public UInterface
{
	GENERATED_BODY()
};

class LANDVEHICULE_API IVehiclesInteractions
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Interact(APawn* PlayerInteract);
};
