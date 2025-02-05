#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Interface/VehiclesInteractions.h"
#include "CustomPlayerController.generated.h"

class AVehicleMaster;
class UInputMappingContext;

UCLASS()
class LANDVEHICULE_API ACustomPlayerController : public APlayerController, public IVehiclesInteractions
{
	GENERATED_BODY()

public:

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(Client, Reliable, BlueprintCallable)
	void Client_AddMappingContext(const UInputMappingContext* NewMappingContext) const;

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void Client_RemoveMappingContext(const UInputMappingContext* MappingContextToRemove) const;

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_ChangeCamera(AVehicleMaster* Vehicle);

	UFUNCTION(BlueprintCallable)
	void EnterInVehicle(AVehicleMaster* Vehicle);
	UFUNCTION(Server, Reliable)
	void Server_EnterInVehicle(AVehicleMaster* Vehicle);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_OutOfVehicle(AVehicleMaster* Vehicle);
	
	UPROPERTY(Replicated, BlueprintReadWrite, ReplicatedUsing = OnRep_CurrentVehicle, Category = "Vehicle")
	bool bInVehicle;

	UPROPERTY(Replicated, BlueprintReadWrite, ReplicatedUsing = OnRep_bInVehicle, Category = "Vehicle")
	AVehicleMaster* CurrentVehicle;

	UFUNCTION()
	void OnRep_CurrentVehicle();
	UFUNCTION()
	void OnRep_bInVehicle();
};
