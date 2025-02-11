#pragma once

#include "CoreMinimal.h"
#include "GameUIManagerSubsystem.h"
#include "SGameUIManagerSubsystem.generated.h"

UCLASS()
class MULTIPLAYERMENU_API USGameUIManagerSubsystem : public UGameUIManagerSubsystem
{
	GENERATED_BODY()

public:
	USGameUIManagerSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	bool Tick(float DeltaTime);
	void SyncRootLayoutVisibilityToShowHUD();

	FTSTicker::FDelegateHandle TickHandler;
};
