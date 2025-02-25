#pragma once

#include "CoreMinimal.h"
#include "CommonPlayerController.h"
#include "SPlayerController.generated.h"

class USMenuMultiplayerWidget;
class UCommonActivatableWidget;

UCLASS()
class MULTIPLAYERMENU_API ASPlayerController : public ACommonPlayerController
{
	GENERATED_BODY()
	
public:
	ASPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void OnPossess(APawn* InPawn) override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void ShowMenu(const TSubclassOf<UCommonActivatableWidget> MenuClass);

	UPROPERTY(EditAnywhere)
	TSubclassOf<USMenuMultiplayerWidget> MenuWidgetClass;
};
