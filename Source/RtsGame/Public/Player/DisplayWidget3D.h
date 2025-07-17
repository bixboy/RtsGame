#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DisplayWidget3D.generated.h"


class UWidgetComponent;

UCLASS()
class RTSGAME_API ADisplayWidget3D : public AActor
{
	GENERATED_BODY()

public:
	ADisplayWidget3D();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	
	UFUNCTION()
	void AlignWidgetToCamera();

	UPROPERTY()
	UWidgetComponent* WidgetComponent;
};
