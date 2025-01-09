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
	UPROPERTY(BlueprintReadWrite)
	bool bAltIsPressed = false;

#pragma region Camera Movement
protected:	
	UFUNCTION(BlueprintCallable)
	void MoveForward(float Value);
	UFUNCTION(BlueprintCallable)
	void MoveRight(float Value);
	UFUNCTION(BlueprintCallable)
	void Zoom(float Value);

	UFUNCTION(BlueprintCallable)
	void RotateRight();
	UFUNCTION(BlueprintCallable)
	void RotateLeft();

	UFUNCTION(BlueprintCallable)
	void EnableRotation();
	UFUNCTION(BlueprintCallable)
	void DisableRotation();

	UFUNCTION(BlueprintCallable)
	void RotateHorizontal(float Value);
	UFUNCTION(BlueprintCallable)
	void RotateVertical(float Value);

#pragma endregion	

#pragma region Movement Utiliti
protected:	
	UFUNCTION()
	void GetTerrainPosition(FVector& TerrainPosition);
	UFUNCTION()
	void EdgeScroll();

#pragma endregion	

#pragma region Variables Movement
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

// Selection	
#pragma region Selection
protected:
	/*- Function -*/
	UFUNCTION(BlueprintCallable)
	AActor* GetSelectedObject();

	UFUNCTION(BlueprintCallable)
	void LeftMousePressed();
	UFUNCTION(BlueprintCallable)
	void LeftMouseReleased();
	UFUNCTION(BlueprintCallable)
	void LeftMouseInputHold(float Value);

	UFUNCTION(BlueprintCallable)
	void RightMousePressed();
	UFUNCTION(BlueprintCallable)
	void RightMouseReleased();

	UFUNCTION()
	void CreateSelectionBox();

	/*- Variables -*/
	UPROPERTY()
	TObjectPtr<APlayerControllerRts> Player;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Mouse")
	float LeftMouseHoldThreshold = 0.15f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Mouse")
	TSubclassOf<ASelectionBox> SelectionBoxClass;

	UPROPERTY()
	ASelectionBox* SelectionBox;
	bool BoxSelect;
	FVector LeftMouseHitLocation;

#pragma endregion

// Command
#pragma region Command
protected:
	/*- Function -*/
	UFUNCTION(BlueprintCallable)
	void CommandStart();
	UFUNCTION(BlueprintCallable)
	void Command();
	
	UFUNCTION()
	FCommandData CreatCommandData(const ECommandType Type, AActor* Enemy = nullptr, const float Radius = 0.f) const;
	
	UFUNCTION(BlueprintCallable)
	void AltRightMousePressed();
	UFUNCTION(BlueprintCallable)
	void AltRightMouseReleased();
	UFUNCTION(BlueprintCallable)
	void AltRightMouseHold(float Value);
	
	UFUNCTION()
	void CreatSphereRadius();

	/*- Variables -*/
	UPROPERTY(EditAnywhere, Category = "Settings|Command")
	TSubclassOf<ASphereRadius> SphereRadiusClass;
	UPROPERTY()
	ASphereRadius* SphereRadius;
	UPROPERTY()
	bool SphereRadiusEnable;

#pragma endregion 	
	
private:
	UFUNCTION()
	void CameraBounds();
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	USceneComponent* SceneComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	USpringArmComponent* SpringArm;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	UCameraComponent* CameraComponent;

	UPROPERTY()
	FVector TargetLocation;
	UPROPERTY()
	FRotator TargetRotation;

	UPROPERTY()
	float TargetZoom;
	UPROPERTY()
	bool CanRotate;
	UPROPERTY()
	FVector CommandLocation;
};
