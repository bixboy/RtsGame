#pragma once

#include "CoreMinimal.h"
#include "SPlayerController.generated.h"

class USMenuMultiplayerWidget;
class UCommonActivatableWidget;

UCLASS()
class MULTIPLAYERMENU_API ASPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ASPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void ShowMenu(const TSubclassOf<UCommonActivatableWidget> MenuClass);
	
	UFUNCTION(BlueprintImplementableEvent, Category = Sessions)
	void DestroySession(APlayerController* Controller);

	UPROPERTY(EditAnywhere)
	TSubclassOf<USMenuMultiplayerWidget> MenuWidgetClass;
};
