﻿#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CameraVehicle.generated.h"

class UCameraComponent;

UCLASS()
class LANDVEHICULE_API ACameraVehicle : public APawn
{
	GENERATED_BODY()

public:
	ACameraVehicle();
	
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable)
	UCameraComponent* GetCameraComponent();
	
	UFUNCTION()
	bool GetIsUsed() const;
	
	UFUNCTION()
	void SetIsUsed(bool bIsUsed);
	
	UFUNCTION()
	void SetController(APlayerController* NewOwner);
	
	UFUNCTION()
	APlayerController* GetCameraController() const;

<<<<<<< Updated upstream
	
	// Accessor
	UFUNCTION()
	void SetIsTurret(bool NewBool);

	UFUNCTION()
	void SetSwitchToOtherTypeCam(bool NewBool);
	
	UFUNCTION()
	bool GetIsTurret();

	UFUNCTION()
	bool GetSwitchToOtherTypeCam();

	
	UPROPERTY(Replicated)
	FTurrets Turret;

=======
>>>>>>> Stashed changes
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComp;

	UPROPERTY()
	bool IsUsed = false;

	UPROPERTY(Replicated)
	bool bIsTurrets = true;

	UPROPERTY(Replicated)
	bool bSwitchToOtherTypeCam = true;
	
	UPROPERTY()
	APlayerController* OwningPlayer;
};
