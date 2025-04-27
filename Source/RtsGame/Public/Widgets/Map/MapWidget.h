#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MapWidget.generated.h"

class UImage;
class UBorder;
class UMapIconComponent;
class ADynamicCameraMap;
class UCanvasPanel;


UCLASS()
class RTSGAME_API UMapWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;
	
	virtual void NativeConstruct() override;
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:

	UFUNCTION()
	void AddIcon(UMapIconComponent* Comp);

	UFUNCTION()
	void RemoveIcon(UMapIconComponent* Comp);

	
	UPROPERTY(meta = (BindWidget))
	UBorder* MinimapImage;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* IconCanvas;

	UPROPERTY(EditAnywhere, Category="Settings")
	UTexture2D* DefaultIcon;

	UPROPERTY(EditAnywhere, Category="Settings")
	float MiniMapSize = 380.f;

private:
	UPROPERTY()
	ADynamicCameraMap* MapCam = nullptr;

	UPROPERTY()
	TMap<UMapIconComponent*, UImage*> IconMap;

	UPROPERTY()
	FVector2D WorldOrig;

	UPROPERTY()
	float WorldSize;

	UPROPERTY()
	FVector2D ImageSize;
};
