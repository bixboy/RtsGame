#pragma once
#include "CoreMinimal.h"
#include "Data/DataRts.h"
#include "Player/PlayerControllerRts.h"
#include "RtsPlayerController.generated.h"

class URtsResourcesComponent;
class URtsComponent;


UCLASS()
class RTSGAME_API ARtsPlayerController : public APlayerControllerRts
{
	GENERATED_BODY()
	
public:
	ARtsPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	URtsComponent* RtsComponent;

// ---------- Teams
	UFUNCTION()
    int GetPlayerTeam();

    UFUNCTION()
    void SetPlayerTeam(int NewPlayerTeam);
	

// ---------- Resources	
	UFUNCTION()
	void OnNewResources(const FResourcesCost& NewResources);

	UFUNCTION()
	void AddResource(FResourcesCost NewResource);

	UFUNCTION()
	void RemoveResource(FResourcesCost RemoveResource);

	UPROPERTY(EditAnywhere)
	URtsResourcesComponent* ResourcesComponent;

protected:
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(Replicated)
	int CurrentTeam;
};
