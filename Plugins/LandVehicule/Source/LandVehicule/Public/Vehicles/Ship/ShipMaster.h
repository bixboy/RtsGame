#pragma once
#include "CoreMinimal.h"
#include "Vehicles/VehicleMaster.h"
#include "ShipMaster.generated.h"


UCLASS()
class LANDVEHICULE_API AShipMaster : public AVehicleMaster
{
	GENERATED_BODY()

public:
	AShipMaster();

protected:
	virtual void BeginPlay() override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Tick(float DeltaTime) override;

	virtual void Server_SwitchEngine(bool OnOff) override;

	
	UFUNCTION()
	void Input_Thrust(const FInputActionValue& InputActionValue);

	UFUNCTION(Server, Reliable)
	void Server_Thrust(FInputActionValue InputActionValue);
	
	UFUNCTION()
	void OnShipMove(float NewForwardInput, float NewRightInput);

	UFUNCTION()
	void Input_OnBoost();

	UFUNCTION(Server, Reliable)
	void Server_OnBoost();

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Input",  Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> BoostAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Input",  Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> ThrustAction;
	

	// ===== Hovering =====
	UPROPERTY(EditAnywhere, Category="Settings|Ship|Hover")
	float HoverHeight = 500.f;

	UPROPERTY(EditAnywhere, Category="Settings|Ship|Hover")
	float HoverStiffness = 20000.f;

	UPROPERTY(EditAnywhere, Category="Settings|Ship|Hover")
	float HoverDamping = 4000.f;

	UPROPERTY(Replicated)
	bool bHoverActive = false;
	

	// ===== Thrust =====
	UPROPERTY(EditAnywhere, Category="Settings|Ship|Thrust")
	float ThrustIncreaseRate = 500.f;

	UPROPERTY(EditAnywhere, Category="Settings|Ship|Thrust")
	float ThrustDecreaseRate = 800.f;

	UPROPERTY(EditAnywhere, Category="Settings|Ship|Thrust")
	float ThrustInterpSpeed = 5.f;

	UPROPERTY(EditAnywhere, Category="Settings|Ship|Thrust")
	float MaxThrust = 2000.f;

	UPROPERTY()
	float Thrust = 0.f;

	UPROPERTY()
	float ThrustInput = 0.f;

	
	// ===== Rotation =====
	UPROPERTY(EditAnywhere, Category = "Settings|Ship|Rotation")
	float RotationTorque = 100.f;

	UPROPERTY(EditAnywhere, Category = "Settings|Ship|Rotation")
	float ForwardTorquePower = 40.f;

	UPROPERTY(EditAnywhere, Category = "Settings|Ship|Rotation")
	float SideTorquePower = 150.f;

	

	UPROPERTY(EditAnywhere, Category = "Settings|Ship")
	float GravityThreshold = 1000.f;

	UPROPERTY()
	FVector ForwardVelocity;

	UPROPERTY()
	FVector UpWardVelocity;

	UPROPERTY(Replicated)
	bool bSuperSpeed;
};
