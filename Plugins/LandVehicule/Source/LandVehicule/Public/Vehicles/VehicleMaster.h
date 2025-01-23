#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Interface/VehiclesInteractions.h"
#include "VehicleMaster.generated.h"

class UInputMappingContext;
class UVehiclesAnimInstance;
class ACameraVehicle;

UCLASS()
class LANDVEHICULE_API AVehicleMaster : public APawn, public IVehiclesInteractions
{
	GENERATED_BODY()

public:
	AVehicleMaster();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

protected:
	virtual void BeginPlay() override;

#pragma region Components

protected:
	/*- Variables -*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* BaseVehicle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = true))
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
	UCameraComponent* MainCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta=(AllowPrivateAccess=true))
	USkeletalMeshComponent* SkeletalBaseVehicle;

	UPROPERTY()
	UVehiclesAnimInstance* AnimInstance;

#pragma endregion

#pragma region Camera System

protected:
	/*- Variables -*/
	UPROPERTY(EditAnywhere, Category = "Settings|Camera")
	float CameraDistance = 700.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = true))
	TArray<ACameraVehicle*> Cameras;

	UPROPERTY()
	ACameraVehicle* CurrentCamera;

	/*- Functions -*/
	UFUNCTION()
	void InitializeCameras();
	
	UFUNCTION(BlueprintCallable)
	void SwitchToCamera(APlayerController* PlayerController, ACameraVehicle* NewCamera);

	UFUNCTION(BlueprintCallable)
	void SwitchToNextCamera(APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	ACameraVehicle* GeCameraInArray(int Index);

#pragma endregion

#pragma region Movement System

protected:
	/*- Variables -*/
	UPROPERTY()
	float ForwardInput = 0.0f;

	UPROPERTY()
	float TurnInput = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Settings|Movement")
	float MaxSpeed = 3000.0f;

	/*- Functions -*/
	UFUNCTION(BlueprintCallable)
	void MoveForward(float Value);

	UFUNCTION(BlueprintCallable)
	void MoveRight(float Value);

public:
	// Getters pour les entrées de mouvement
	UFUNCTION()
	float GetForwardInput() const;

	UFUNCTION()
	float GetTurnInput() const;

#pragma endregion

#pragma region Vehicle Settings

protected:
	/*- Variables -*/
	UPROPERTY(EditAnywhere, Category = "Settings|Vehicle")
	bool EngineOn = false;

	UPROPERTY(EditAnywhere, Category = "Settings|Vehicle")
	int PlacesNumber;

	UPROPERTY(EditAnywhere, Category = "Settings|Vehicle")
	UInputMappingContext* NewMappingContext;

	UPROPERTY()
	ACharacter* CurrentDriver;

	/*- Functions -*/
	UFUNCTION(BlueprintCallable)
	void TurnEngineOn(bool OnOff);

#pragma endregion

#pragma region Sounds

protected:
	/*- Variables -*/
	UPROPERTY(EditAnywhere, Category = "Settings|Sounds")
	USoundBase* SoundEngineOn;

	UPROPERTY(EditAnywhere, Category = "Settings|Sounds")
	USoundBase* SoundMoveForward;

	UPROPERTY(EditAnywhere, Category = "Settings|Sounds")
	USoundBase* SoundBrake;

	/*- Functions -*/
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "PlaySound"))
	void PlaySound(USoundBase* Sound);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "StopSound"))
	void StopSound();

#pragma endregion

#pragma region Interfaces

public:
	UFUNCTION()
	virtual void Interact_Implementation(APawn* PlayerInteract) override;

#pragma endregion
};