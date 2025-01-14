#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "VehicleMaster.generated.h"

UCLASS()
class LANDVEHICULE_API AVehicleMaster : public APawn
{
	GENERATED_BODY()

public:
	AVehicleMaster();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="PlaySond"))
	void PlaySound(USoundBase* Sound);
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="StopSond"))
	void StopSound();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta=(AllowPrivateAccess=true))
	UStaticMeshComponent* BaseVehicle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta=(AllowPrivateAccess=true))
	USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta=(AllowPrivateAccess=true))
	UCameraComponent* Camera;

protected:
	/*- Camera Variables -*/
	UPROPERTY(EditAnywhere, Category = "Settings|Camera")
	float CameraDistance = 700.f;

	/*- Movement Vehicle -*/
	UFUNCTION(BlueprintCallable)
	void MoveForward(float Value);
	UFUNCTION(BlueprintCallable)
	void MoveRight(float Value);
	
	UPROPERTY()
	float ForwardInput = 0.0f;
	UPROPERTY()
	float TurnInput = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Settings|Movement")
	float MaxSpeed = 3000.0f;

	UPROPERTY(EditAnywhere, Category = "Settings|Vehicle")
	bool EngineOn = false;
	UPROPERTY(EditAnywhere, Category = "Settings|Vehicle")
	int PlacesNumber;

	/*- Sounds -*/
	UPROPERTY(EditAnywhere, Category = "Settings|Sounds")
	USoundBase* SoundEngineOn;
	UPROPERTY(EditAnywhere, Category = "Settings|Sounds")
	USoundBase* SoundMoveForward;
	UPROPERTY(EditAnywhere, Category = "Settings|Sounds")
	USoundBase* SoundBrake;

	/*- Function -*/
	UFUNCTION(BlueprintCallable)
	void TurnEngineOn(bool OnOff);
	
public:
	/*- Getter -*/
	UFUNCTION()
	float GetForwardInput() const;
	UFUNCTION()
	float GetTurnInput() const;
};
