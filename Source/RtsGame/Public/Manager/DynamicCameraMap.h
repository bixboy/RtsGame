#pragma once
#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "Components/MapIconComponent.h"
#include "DynamicCameraMap.generated.h"


UCLASS()
class RTSGAME_API ADynamicCameraMap : public ACameraActor
{
	GENERATED_BODY()

public:
	ADynamicCameraMap();

	UFUNCTION()
	void RegisterMapIcon(UMapIconComponent* Comp);

	UFUNCTION()
	void UnregisterMapIcon(UMapIconComponent* Comp);

	UFUNCTION()
	const TSet<TWeakObjectPtr<UMapIconComponent>>& GetMapIcons() const { return RegisteredIcons; }

	UPROPERTY(EditAnywhere, Category="Settings")
	float OrthoWidth = 5000.f;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void UpdateRenderTarget();

	UPROPERTY(VisibleAnywhere, Category="Settings")
	USceneCaptureComponent2D* SceneCaptureComp;

	UPROPERTY(EditAnywhere, Category="Settings")
	TArray<TSubclassOf<AActor>> ClassesToHide;

	UPROPERTY(EditAnywhere, Category="Settings")
	UTextureRenderTarget2D* MinimapRenderTarget;

	UPROPERTY()
	TSet<TWeakObjectPtr<UMapIconComponent>> RegisteredIcons;
	
};
