#pragma once
#include "CoreMinimal.h"
#include "Player/PlayerCamera.h"
#include "Structures/StructureBase.h"
#include "RtsPlayer.generated.h"


class AStructurePreview;
class ARtsPlayerController;

UCLASS()
class RTSGAME_API ARtsPlayer : public APlayerCamera
{
	GENERATED_BODY()

public:
	ARtsPlayer();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	virtual void Input_SelectAllUnitType() override;
	
	virtual void CreatePreviewMesh() override;

	virtual void ShowUnitPreview(TSubclassOf<ASoldierRts> NewUnitClass) override;

	virtual void HidePreview() override;

	virtual void Input_OnSpawnUnits() override;

	UFUNCTION()
	void ShowBuildPreview(const FStructure BuildData);

	UPROPERTY()
	ARtsPlayerController* RtsController;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Preview")
	TSubclassOf<AStructurePreview> BuildPreviewClass;
	
	UPROPERTY()
	AStructurePreview* Preview;

	UPROPERTY()
	bool bIsInSpawnBuild = false;
};
