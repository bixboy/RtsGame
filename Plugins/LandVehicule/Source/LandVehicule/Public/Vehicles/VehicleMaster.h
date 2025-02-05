﻿#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Interface/VehiclesInteractions.h"
#include "VehicleMaster.generated.h"

class UInputMappingContext;
class UVehiclesAnimInstance;
class ACameraVehicle;

UENUM(BlueprintType)
enum class EVehiclePlaceType : uint8
{
	Driver,
	Gunner,
	None
};

USTRUCT(BlueprintType)
struct FVehicleRole
{
	GENERATED_BODY()

	UPROPERTY()
	APawn* Player = nullptr;

	UPROPERTY()
	EVehiclePlaceType RoleName;

	bool operator==(const FVehicleRole& Other) const
	{
		return	Player == Other.Player &&
				RoleName == Other.RoleName;
	}
};

USTRUCT(BlueprintType)
struct FTurrets
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	ACameraVehicle* CameraVehicle = nullptr;

	UPROPERTY(BlueprintReadWrite)
	float AccumulatedYaw = 0.f;
	UPROPERTY(BlueprintReadWrite)
	float AccumulatedPitch = 0.f;
	
	bool operator==(const FTurrets& Other) const
	{
		return CameraVehicle == Other.CameraVehicle && 
			   AccumulatedYaw == Other.AccumulatedYaw && 
			   AccumulatedPitch == Other.AccumulatedPitch;
	}
};

UCLASS()
class LANDVEHICULE_API AVehicleMaster : public APawn, public IVehiclesInteractions
{
	GENERATED_BODY()

public:
	AVehicleMaster();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PossessedBy(AController* NewController) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	TMap<APlayerController*, ACameraVehicle*> PlayersInVehicle;

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
	TArray<FTurrets> Turrets;

	UPROPERTY()
	FTurrets CurrentCamera;

	/*- Functions -*/
	UFUNCTION()
	void InitializeCameras();
	
	UFUNCTION(BlueprintCallable)
	void SwitchToCamera(APlayerController* PlayerController, const FTurrets& NewCamera);
	UFUNCTION(Server, Reliable)
	void Server_SwitchToCamera(APlayerController* PlayerController, const FTurrets& NewCamera);
	UFUNCTION(Client, Reliable)
	void Client_SwitchToCamera(APlayerController* PlayerController, const FTurrets& NewCamera);

	UFUNCTION(BlueprintCallable)
	void SwitchToNextCamera(APlayerController* Player);
	
	UFUNCTION()
	void SwitchToMainCam(APlayerController* PlayerController);
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	ACameraVehicle* GetAttachedCamera(FName ParentName);

	UFUNCTION()
	int GetCameraIndex(const ACameraVehicle* CameraVehicle);
	
	UFUNCTION()
	FTurrets GetAvailableCamera(int startIndex);

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

	UPROPERTY()
	int CurrentPlace;

	UPROPERTY(EditAnywhere, Category = "Settings|Vehicle")
	UInputMappingContext* NewMappingContext;

	UPROPERTY(BlueprintReadOnly, Replicated)
	APawn* CurrentDriver;

	UPROPERTY(EditAnywhere, Category = "Settings|Vehicle")
	bool HaveTurret = false;

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
	virtual bool Interact_Implementation(ACustomPlayerController* PlayerInteract) override;

	UFUNCTION()
	virtual void UpdateTurretRotation_Implementation(FVector2D Rotation, FName TurretName) override;

	UFUNCTION()
	virtual ACameraVehicle* GetCurrentCameraVehicle_Implementation() override;

	UFUNCTION()
	virtual void ChangePlace_Implementation(ACustomPlayerController* Player) override;

	UFUNCTION()
	virtual void OutOfVehicle_Implementation(ACustomPlayerController* PlayerController) override;

#pragma endregion

#pragma region Turret System
	
protected:
	/*- Variables -*/
	UPROPERTY(Replicated)
	TArray<FVehicleRole> VehicleRoles;

	UPROPERTY(EditAnywhere, Category = "Settings|Turret")
	TArray<TObjectPtr<UStaticMeshComponent>> SmTurrets;
	
	UPROPERTY()
	FRotator CurrentAngle;

	UPROPERTY(EditAnywhere, Category = "Settings|Turret")
	float TurretRotationSpeed = 0.07f;

	/*- Functions -*/
	UFUNCTION()
	void AssignRole(APawn* Player, EVehiclePlaceType RoleName);
	
	UFUNCTION(BlueprintCallable)
	APawn* GetPlayerForRole(EVehiclePlaceType RoleName) const;
	UFUNCTION()
	EVehiclePlaceType GetRoleByPlayer(const APawn* Player) const;

	UFUNCTION()
	void ReleaseRole(APawn* Player);

	UFUNCTION(BlueprintCallable)
	void SetTurretRotation(ACameraVehicle* Camera, FRotator TurretAngle);

	UFUNCTION(BlueprintCallable)
	void ApplyTurretRotation(float DeltaYaw, float DeltaPitch, float RotationSpeed, float DeltaTime);

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FRotator GetTurretAngle();
	
#pragma endregion	
};