#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BuildProgressWidget.generated.h"

class UBuildInfoBox;
class AStructureBase;
class UTextBlock;
class UProgressBar;


UCLASS()
class RTSGAME_API UBuildProgressWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void UpdateProgress(AActor* NewBuild);

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	UFUNCTION()
	void NativeOnInitialized() override;
	
	UFUNCTION()
	void OnBuildStart(float Progress);

	UFUNCTION()
	void OnBuildStop();

	UFUNCTION()
	void OnBuildCompleted();
	

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UProgressBar* ProgressBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* ProgressText;


	UPROPERTY()
	AStructureBase* Build = nullptr;

	UPROPERTY()
	UBuildInfoBox* BuildInfoBox;
	

	UPROPERTY()
	bool bUpdatePercent = false;

	UPROPERTY()
	float SimulatedElapsed = 0.f;

	UPROPERTY()
	float TotalBuildTime = 1.f;
};
