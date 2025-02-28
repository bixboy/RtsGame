#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "SGameModeLobby.generated.h"


UCLASS()
class MULTIPLAYERMENU_API ASGameModeLobby : public AGameMode
{
	GENERATED_BODY()

public:

	ASGameModeLobby(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void OnPostLogin(AController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;

protected:
	UPROPERTY()
	TArray<APlayerController*> Controllers;
};
