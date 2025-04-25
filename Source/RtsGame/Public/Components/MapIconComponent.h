#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MapIconComponent.generated.h"

class ADynamicCameraMap;
class UMapIconWidget;


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RTSGAME_API UMapIconComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMapIconComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
	UTexture2D* IconTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
	FVector2D IconSize = FVector2D(16,16);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings")
	bool bCanShow = true;

protected:
	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY()
	ADynamicCameraMap* CachedMapCam = nullptr;
};
