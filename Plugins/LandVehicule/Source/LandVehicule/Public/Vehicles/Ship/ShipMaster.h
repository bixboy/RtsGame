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

	UFUNCTION(Server, Reliable)
	void Server_OnShipYaw(const FInputActionValue& InputActionValue);

	UFUNCTION(Server, Reliable)
	void Server_OnShipLift(const FInputActionValue& InputActionValue);

	UFUNCTION()
	void Input_OnBoost();

	UFUNCTION(Server, Reliable)
	void Server_OnBoost();

	UFUNCTION()
	void EndBoost();

	UFUNCTION()
	void AttemptLanding();

	UFUNCTION()
	void ShipLanding(float DeltaTime);

	UFUNCTION()
	void TakeOff();


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Ship|Input",  Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> BoostAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Ship|Input",  Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> ThrustAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Ship|Input",  Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> PivoAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Ship|Input",  Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> LiftAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Ship|Input",  Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> LandingAction;
	

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


	// ===== Speed Factor =====
	UPROPERTY(EditAnywhere, Category="Settings|Ship|SpeedFactor")
	float MinUpSpeedFactor = 0.8f;

	UPROPERTY(EditAnywhere, Category="Settings|Ship|SpeedFactor")
	float MaxDownSpeedFactor = 1.5f;

	
	// ===== Rotation =====
	UPROPERTY(EditAnywhere, Category="Settings|Ship|Rotation")
	float MaxRollSpeedMultiplier = 3.f;
	
	UPROPERTY(EditAnywhere, Category = "Settings|Ship|Rotation")
	float RotationTorque = 100.f;

	UPROPERTY(EditAnywhere, Category = "Settings|Ship|Rotation")
	float ForwardTorquePower = 40.f;

	UPROPERTY(EditAnywhere, Category = "Settings|Ship|Rotation")
	float SideTorquePower = 100.f;


	// ===== Boost =====
	UPROPERTY(EditAnywhere, Category="Settings|Ship|Boost")
	float BoostMultiplier = 2.0f;
	
	UPROPERTY(EditAnywhere, Category="Settings|Ship|Boost", meta = (ClampMin="0.1", UIMin="0.1", UIMax="10.0"))
	float BoostDuration = 3.0f;

	UPROPERTY(Replicated)
	bool bSuperSpeed;
	
	UPROPERTY()
	FTimerHandle BoostTimerHandle;


	// ===== Landing =====
	UPROPERTY(EditAnywhere, Category="Settings|Ship|Landing")
	float LandingDistanceThreshold = 500.f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.1", ClampMax = "1"), Category="Settings|Ship|Landing")
	float LandingMaxThrustFactor = 0.1f;

	UPROPERTY(EditAnywhere, Category="Settings|Ship|Landing")
	float LandingSpeed = 300.f;

	UPROPERTY(EditAnywhere, Category="Settings|Ship|Landing")
	float LandingRotationSpeed = 90.f;

	UPROPERTY(EditAnywhere, Category="Settings|Ship|Landing")
	float LegContactDistance = 20.f;
	
	UPROPERTY()
	TArray<USceneComponent*> LandingGears;

	UPROPERTY()
	bool bIsLanded = true;

	UPROPERTY()
	bool bIsLanding = false;
	
	UPROPERTY()
	FRotator InitialRotation;

	UPROPERTY()
	float LandingStartZ;

	UPROPERTY()
	float TotalLandingDistance;

	
	
	UPROPERTY(EditAnywhere, Category = "Settings|Ship")
	float GravityThreshold = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Settings|Ship")
	float LiftForcePower = 150.f;

	UPROPERTY()
	FVector ForwardVelocity;

	UPROPERTY()
	FVector UpWardVelocity;
};
