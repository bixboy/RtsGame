#pragma once

#include "CoreMinimal.h"
#include "Data/AiData.h"
#include "GameFramework/Pawn.h"
#include "PlayerCamera.generated.h"

class ASphereRadius;
class ASelectionBox;
class APlayerControllerRts;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class RTSMODE_API APlayerCamera : public APawn
{
	GENERATED_BODY()

public:
	APlayerCamera();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	APlayerControllerRts* GetRtsPlayerController();

private:
	UFUNCTION()
	void CameraBounds();

#pragma region Camera Components
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	USceneComponent* SceneComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	USpringArmComponent* SpringArm;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	UCameraComponent* CameraComponent;
#pragma endregion

#pragma region Camera Movement
	
protected:
	/** Déplacements de la caméra */
	UFUNCTION(BlueprintCallable)
	void MoveForward(float Value);
	UFUNCTION(BlueprintCallable)
	void MoveRight(float Value);
	UFUNCTION(BlueprintCallable)
	void Zoom(float Value);

	/** Rotation de la caméra */
	UFUNCTION(BlueprintCallable)
	void RotateCamera(float Angle);
	UFUNCTION(BlueprintCallable)
	void EnableRotation(bool bRotate);
	

	/** Gestion du Edge Scroll */
	UFUNCTION()
	void EdgeScroll();

	/** Position ciblée de la caméra */
	UPROPERTY()
	FVector TargetLocation;
	UPROPERTY()
	FRotator TargetRotation;
	UPROPERTY()
	float TargetZoom;
	UPROPERTY()
	bool CanRotate;

	UFUNCTION()
	void GetTerrainPosition(FVector& TerrainPosition) const;
	
#pragma endregion

#pragma region Camera Settings
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float CameraSpeed = 20.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float EdgeScrollSpeed = 2.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float RotateSpeed = 2.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float RotatePitchMin = 10.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float RotatePitchMax = 80.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float ZoomSpeed = 2.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float MinZoom = 500.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float MaxZoom = 4000.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	bool CanEdgeScroll = false;
#pragma endregion

#pragma region Selection
protected:
	UFUNCTION(BlueprintCallable)
	AActor* GetSelectedObject();

	UFUNCTION(BlueprintCallable)
	void HandleLeftMouse(EInputEvent InputEvent, float Value);
	
	UFUNCTION(BlueprintCallable)
	void HandleAltRightMouse(EInputEvent InputEvent, float Value);

	UFUNCTION()
	void CreateSelectionBox();
	UFUNCTION()
	void CreateSphereRadius();

	UPROPERTY()
	APlayerControllerRts* Player;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Mouse")
	float LeftMouseHoldThreshold = 0.15f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Mouse")
	TSubclassOf<ASelectionBox> SelectionBoxClass;
	
	UPROPERTY()
	ASelectionBox* SelectionBox;

	UPROPERTY()
	bool BoxSelect;
	
	UPROPERTY()
	FVector LeftMouseHitLocation;

	UPROPERTY(BlueprintReadWrite)
	bool bAltIsPressed = false;
#pragma endregion

#pragma region Command
protected:
	/** Démarre une commande */
	UFUNCTION(BlueprintCallable)
	void CommandStart();

	/** Exécute une commande */
	UFUNCTION(BlueprintCallable)
	void Command();

	/** Crée une commande avec des paramètres */
	UFUNCTION()
	FCommandData CreateCommandData(const ECommandType Type, AActor* Enemy = nullptr, float Radius = 0.f) const;

	/** Variables pour les commandes */
	UPROPERTY(EditAnywhere, Category = "Settings|Command")
	TSubclassOf<ASphereRadius> SphereRadiusClass;
	UPROPERTY()
	ASphereRadius* SphereRadius;
	UPROPERTY()
	bool SphereRadiusEnable;
	UPROPERTY()
	FVector CommandLocation;
#pragma endregion
};