#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CustomPlayerController.generated.h"

class UInputMappingContext;

UCLASS()
class LANDVEHICULE_API ACustomPlayerController : public APlayerController
{
	GENERATED_BODY()

public:	
	UFUNCTION(Client, Reliable)
	void Client_AddMappingContext(const UInputMappingContext* NewMappingContext) const;

	UFUNCTION(Client, Reliable)
	void Client_RemoveMappingContext(const UInputMappingContext* MappingContextToRemove) const;
};
