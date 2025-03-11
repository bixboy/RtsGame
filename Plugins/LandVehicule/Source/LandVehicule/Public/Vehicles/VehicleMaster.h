#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Camera/CameraComponent.h"
#include "Data/VehicleData.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Interface/VehiclesInteractions.h"
#include "VehicleMaster.generated.h"

class UInputAction;
class UInputMappingContext;
class UVehiclesAnimInstance;
class ACameraVehicle;


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

	UFUNCTION()
	void Input_OnInteract();

	UFUNCTION()
	void Input_OnChangePlace();

//---------------------------- Inputs ----------------------------
#pragma region Inputs
	UFUNCTION()
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, Category = "Settings|Vehicle")
	UInputMappingContext* NewMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Vehicle|Inputs",  Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Vehicle|Inputs",  Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Vehicle|Inputs",  Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> ChangePlaceAction;

#pragma endregion	

//---------------------------- Components ----------------------------
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

//---------------------------- Cameras ----------------------------
#pragma region Camera System

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	ACameraVehicle* GetCurrentCamera();

protected:
	/*- Variables -*/
	UPROPERTY(EditAnywhere, Category = "Settings|Camera")
	float CameraDistance = 700.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = true))
	TArray<ACameraVehicle*> Turrets;

	UPROPERTY()
	ACameraVehicle* CurrentCamera;

	/*- Functions -*/
	UFUNCTION()
	void InitializeCameras();
	
	UFUNCTION(BlueprintCallable)
	void SwitchToCamera(APlayerController* PlayerController, ACameraVehicle* NewCamera);
	
	UFUNCTION(Server, Reliable)
	void Server_SwitchToCamera(APlayerController* PlayerController, ACameraVehicle* NewCamera);
	
	UFUNCTION(Client, Reliable)
	void Client_SwitchToCamera(APlayerController* PlayerController, ACameraVehicle* NewCamera);

	UFUNCTION(BlueprintCallable)
	ACameraVehicle* SwitchToNextCamera(APlayerController* Player);
	
	UFUNCTION()
	void SwitchToMainCam(APlayerController* PlayerController);
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	ACameraVehicle* GetAttachedCamera(FName ParentName);
	
	UFUNCTION()
	ACameraVehicle* GetAvailableCamera(int startIndex);

#pragma endregion

//---------------------------- Movement ----------------------------
#pragma region Movement System

protected:
	/*- Variables -*/
	UPROPERTY(Replicated)
	float ForwardInput = 0.0f;

	UPROPERTY(Replicated)
	float TurnInput = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Settings|Movement")
	float MaxSpeed = 3000.0f;

	/*- Function -*/
	UFUNCTION()
	virtual void Input_OnMove(const FInputActionValue& InputActionValue);

	UFUNCTION(Server, Reliable)
	void Server_OnMove(float InForward, float InTurn);

public:
	// Getters pour les entrées de mouvement
	UFUNCTION()
	float GetForwardInput() const;

	UFUNCTION()
	float GetTurnInput() const;

#pragma endregion

//---------------------------- Settings ----------------------------
#pragma region Vehicle Settings

protected:
	/*- Variables -*/
	UPROPERTY(Replicated, EditAnywhere, Category = "Settings|Vehicle")
	bool EngineOn = false;

	UPROPERTY(EditAnywhere, Category = "Settings|Vehicle")
	int PlacesNumber;

	UPROPERTY()
	int CurrentPlace;

	UPROPERTY(BlueprintReadOnly, Replicated)
	APawn* CurrentDriver;

	UPROPERTY(EditAnywhere, Category = "Settings|Vehicle")
	bool bHaveTurret = false;

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
	virtual ACameraVehicle* GetCurrentCameraVehicle_Implementation() override;

	UFUNCTION()
	virtual ACameraVehicle* ChangePlace_Implementation(ACustomPlayerController* Player) override;

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

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void Multicast_SetTurretRotation(ACameraVehicle* Camera, int IndexOfCamera, FRotator TurretAngle);

	UFUNCTION(BlueprintCallable)
	void ApplyTurretRotation(float DeltaYaw, float DeltaPitch, float RotationSpeed, float DeltaTime, ACameraVehicle* CameraToMove);

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FRotator GetTurretAngle(ACameraVehicle* Camera);

	UFUNCTION(BlueprintCallable)
	void OnTurretRotate(FVector2D NewRotation, ACameraVehicle* CameraToRotate);
	
#pragma endregion	
};