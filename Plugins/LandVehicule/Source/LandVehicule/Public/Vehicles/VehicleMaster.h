#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Camera/CameraComponent.h"
#include "Data/VehicleData.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Interface/VehiclesInteractions.h"
#include "VehicleMaster.generated.h"

class UVehiclePlayerMesh;
class UInputAction;
class UInputMappingContext;
class UVehiclesAnimInstance;
class ACameraVehicle;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnVehicleMoveDelegate, float, NewForwardInput, float, NewRightInput);


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

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void Tick(float DeltaSeconds) override;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Vehicle|Inputs",  Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Vehicle|Inputs",  Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> SwitchEngine;

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

protected:
	/*- Variables -*/
	UPROPERTY(EditAnywhere, Category = "Settings|Camera")
	float CameraDistance = 700.f;

	UPROPERTY(EditAnywhere, Category = "Settings|Camera")
	float Sensitivity = 1.5f;

	UPROPERTY(EditAnywhere, Category = "Settings|Camera")
	bool bCanRotateCamera = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = true))
	TArray<ACameraVehicle*> Turrets;

	UPROPERTY()
	TMap<APlayerController*, ACameraVehicle*> AssignedCameras;

	UPROPERTY()
	FRotator CameraRotationOffset;

	/*- Functions -*/

	UFUNCTION()
	void Input_OnUpdateCameraRotation(const FInputActionValue& InputActionValue);
	
	UFUNCTION()
	void InitializeCameras();
	
	UFUNCTION(BlueprintCallable)
	void SwitchToCamera(APlayerController* PlayerController, ACameraVehicle* NewCamera);
	
	UFUNCTION(Server, Reliable)
	void Server_SwitchToCamera(APlayerController* PlayerController, ACameraVehicle* NewCamera);

	UFUNCTION(BlueprintCallable)
	ACameraVehicle* SwitchToNextCamera(APlayerController* Player);
	
	UFUNCTION()
	void SwitchToMainCam(APlayerController* PlayerController);
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	ACameraVehicle* GetAttachedCamera(FName ParentName);
	
	UFUNCTION()
	ACameraVehicle* GetAvailableCamera(int startIndex);

#pragma endregion

//---------------------------- Player Mesh ----------------------------
#pragma region Player Mesh
protected:

	UFUNCTION()
	int ShowPlayerMesh(APlayerController* PlayerController, int PlaceNum = -1);
	
	UFUNCTION()
	void HidePlayerMesh(APlayerController* PlayerController, int MeshNumber = 0);

	UFUNCTION()
	UVehiclePlayerMesh* GetPlayerMesh(APlayerController* PlayerController);
	FVector FindClosestExitLocation(APlayerController* PlayerController);

	UPROPERTY()
	TArray<UVehiclePlayerMesh*> VehiclePlayersMesh;

	UPROPERTY()
	TMap<APlayerController*, UVehiclePlayerMesh*> PlayersMeshAssigned;

#pragma endregion	

//---------------------------- Movement ----------------------------
#pragma region Movement System
public:
	UPROPERTY(BlueprintAssignable, Category = MovementSystem)
	FOnVehicleMoveDelegate OnVehicleMove;

protected:
	/*- Function -*/
	UFUNCTION()
	virtual void Input_OnMove(const FInputActionValue& InputActionValue);

	UFUNCTION(Server, Reliable)
	void Server_OnMove(float InForward, float InTurn);
	
	/*- Variables -*/
	UPROPERTY(Replicated)
	float ForwardInput = 0.0f;

	UPROPERTY(Replicated)
	float TurnInput = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Settings|Movement")
	float MaxSpeed = 3000.0f;

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
	
	/*- Functions -*/
	UFUNCTION(Server, Reliable, BlueprintCallable)
	virtual void Server_SwitchEngine(bool OnOff);

	UFUNCTION()
	void Input_SwitchEngine();
	
	/*- Variables -*/
	UPROPERTY(Replicated, EditAnywhere, Category = "Settings|Vehicle")
	bool bEngineOn = false;

	UPROPERTY(EditAnywhere, Category = "Settings|Vehicle")
	int PlacesNumber;

	UPROPERTY(EditAnywhere, Category = "Settings|Vehicle")
	float OutOfVehicleOffset = 50.f;

	UPROPERTY(EditAnywhere, Category = "Settings|Vehicle")
	bool bHaveTurret = false;

	UPROPERTY()
	int CurrentPlace;

	UPROPERTY(BlueprintReadOnly, Replicated)
	APawn* CurrentDriver;

#pragma endregion

	
#pragma region Sounds

protected:
	/*- Variables -*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	UAudioComponent* StartEngineAudio;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	UAudioComponent* EngineAudio;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	UAudioComponent* MovementAudio;
	
	// Sound Settings
	UPROPERTY(EditAnywhere, Category = "Settings|Sounds")
	USoundBase* SoundEngineOn;

	UPROPERTY(EditAnywhere, Category = "Settings|Sounds")
	USoundBase* SoundEngineOff;

	UPROPERTY(EditAnywhere, Category = "Settings|Sounds")
	USoundBase* SoundIdleLoop;

	UPROPERTY(EditAnywhere, Category = "Settings|Sounds")
	USoundBase* SoundStartAcceleration;

	UPROPERTY(EditAnywhere, Category = "Settings|Sounds")
	USoundBase* SoundDriveLoop;

	UPROPERTY(EditAnywhere, Category = "Settings|Sounds")
	USoundBase* SoundBrakeLoop;

	/*- Functions -*/
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlaySound(USoundBase* Sound, UAudioComponent* AudioComp);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_StopSound(UAudioComponent* AudioComp);

	UFUNCTION()
	bool GetSoundIsPlaying(USoundBase* Sound, UAudioComponent* AudioComp);

#pragma endregion

#pragma region Interfaces

public:
	UFUNCTION()
	virtual bool Interact_Implementation(ACustomPlayerController* PlayerInteract) override;

	UFUNCTION()
	virtual ACameraVehicle* ChangePlace_Implementation(ACustomPlayerController* Player) override;

	UFUNCTION()
	virtual void OutOfVehicle_Implementation(ACustomPlayerController* PlayerController) override;

#pragma endregion

#pragma region Turret System
	
protected:

	/*- Functions -*/
	UFUNCTION()
	void AssignRole(APawn* Player, EVehiclePlaceType RoleName);
	
	UFUNCTION(BlueprintCallable)
	APawn* GetPlayerByRole(EVehiclePlaceType RoleName) const;
	
	UFUNCTION()
	EVehiclePlaceType GetRoleByPlayer(const APawn* Player) const;

	UFUNCTION()
	void ReleaseRole(APawn* Player);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void Multicast_SetTurretRotation(ACameraVehicle* Camera, int IndexOfCamera, FRotator TurretAngle);

	UFUNCTION(BlueprintCallable)
	void ApplyTurretRotation(float DeltaYaw, float DeltaPitch, float RotationSpeed, float DeltaTime, ACameraVehicle* CameraToMove);

	
	/*- Variables -*/
	UPROPERTY(Replicated)
	TArray<FVehicleRole> VehicleRoles;

	UPROPERTY(EditAnywhere, Category = "Settings|Turret")
	TArray<TObjectPtr<UStaticMeshComponent>> SmTurrets;
	
	UPROPERTY()
	FRotator CurrentAngle;

	UPROPERTY(EditAnywhere, Category = "Settings|Turret")
	float TurretRotationSpeed = 0.07f;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FRotator GetTurretAngle(ACameraVehicle* Camera);

	UFUNCTION(BlueprintCallable)
	void OnTurretRotate(FVector2D NewRotation, ACameraVehicle* CameraToRotate);
	
#pragma endregion	
};