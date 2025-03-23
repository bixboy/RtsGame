#pragma once
#include "CoreMinimal.h"
#include "Structures/StructureBase.h"
#include "UnitProduction.generated.h"

class UUnitsProductionDataAsset;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnitProducedSignature, AActor*, NewUnit);

UCLASS()
class RTSGAME_API AUnitProduction : public AStructureBase
{
	GENERATED_BODY()

public:
	AUnitProduction();
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// ---------- Functions ----------
	
	UFUNCTION(BlueprintCallable, Category = "Production")
	void AddUnitToQueue(UUnitsProductionDataAsset* NewUnit);

	UFUNCTION(BlueprintCallable, Category = "Production")
	void RemoveUnitFromQueue(UUnitsProductionDataAsset* UnitToRemove);

	UFUNCTION(BlueprintCallable, Category = "Production")
	void StartUnitProduction();

	UFUNCTION(BlueprintCallable, Category = "Production")
	void StopUnitProduction();

	UFUNCTION(BlueprintCallable, Category = "Production")
	float GetProductionProgress();

	UPROPERTY(BlueprintAssignable, Category = "Production")
	FOnUnitProducedSignature OnUnitProduced;

protected:

	// -------------- Component --------------
#pragma region Components

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USceneComponent* spawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USceneComponent* DestPoint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UDecalComponent* DecalCursor;

#pragma endregion	


	/*-------- Production --------*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Production")
	TArray<UUnitsProductionDataAsset*> UnitsList;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Production")
	UUnitsProductionDataAsset* UnitSelected;
	
	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Production")
	TArray<UUnitsProductionDataAsset*> ProductionQueue; 

	/*-------- Prod Time --------*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Production")
	float ProductionProgress = 0.f;

	UPROPERTY()
	float ProductionStartTime = 0.f;

	/*-------- Formation --------*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Formation")
	FVector LastDestination = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Formation")
	int32 FormationUnitCount = 0;

private:
	FTimerHandle ProductionTimerHandle;

	UFUNCTION(Server, Reliable)
	void Server_StartUnitProduction();
	
	UFUNCTION(Server, Reliable)
	void Server_StopUnitProduction();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnUnitProduced(AActor* NewUnit);

	UFUNCTION()
	void OnProductionFinished();

	UFUNCTION()
	FCommandData GetDestination();

	UFUNCTION()
	void OnNewSelected(bool bIsSelected);
};
