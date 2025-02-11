#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerController.generated.h"

class UCommonActivatableWidget;

UCLASS()
class MULTIPLAYERMENU_API ASPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ASPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void OnPossess(APawn* InPawn) override;
	void ShowMenu(const TSoftClassPtr<UCommonActivatableWidget>& MenuClass);
};
