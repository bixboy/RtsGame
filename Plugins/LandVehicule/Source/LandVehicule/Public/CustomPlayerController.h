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

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_OutOfVehicle(AVehicleMaster* Vehicle);

	UFUNCTION(BlueprintCallable)
	void RotateVehicleTurret(FVector2D NewRotation);
	
	UPROPERTY(Replicated, BlueprintReadWrite, ReplicatedUsing = OnRep_CurrentVehicle, Category = "Vehicle")
	bool bInVehicle;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	ACameraVehicle* GetCurrentCamera();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	AVehicleMaster* GetCurrentVehicle();

protected:

	UFUNCTION(Server, Reliable)
	void Server_EnterInVehicle(AVehicleMaster* Vehicle);

	UFUNCTION()
	void OnRep_CurrentVehicle();
	
	UFUNCTION()
	void OnRep_bInVehicle();

	UFUNCTION(Server, Reliable)
	void Server_RotateVehicleTurret(const FVector2D NewRotation);

	UPROPERTY(Replicated, BlueprintReadOnly)
	ACameraVehicle* CurrentCamera;

	UPROPERTY(Replicated, BlueprintReadWrite, ReplicatedUsing = OnRep_bInVehicle, Category = "Vehicle")
	AVehicleMaster* CurrentVehicle;
	
};
