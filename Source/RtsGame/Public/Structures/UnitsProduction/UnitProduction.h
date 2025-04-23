#pragma once
#include "CoreMinimal.h"
#include "Interfaces/UnitProductionInterface.h"
#include "Structures/StructureBase.h"
#include "UnitProduction.generated.h"

class UUnitsProductionDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnitProducedSignature, AActor*, NewUnit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnProductionProgress, float, NewProgress, UUnitsProductionDataAsset*, UnitInProduct);


UCLASS()
class RTSGAME_API AUnitProduction : public AStructureBase, public IUnitProductionInterface
{
	GENERATED_BODY()

public:
	AUnitProduction();
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void AddUnitToProduction_Implementation(UUnitsProductionDataAsset* NewUnit);

	virtual TArray<UUnitsProductionDataAsset*> GetUnitsProduction_Implementation();

	virtual TArray<UUnitsProductionDataAsset*> GetUnitsInQueueByClass_Implementation(TSubclassOf<AActor> FilterClass);

	virtual TArray<UUnitsProductionDataAsset*> GetProductionList_Implementation();

	virtual float GetProductionProgress_Implementation() override;

	// ---------- Functions ----------
	
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Production")
	void Server_AddUnitToQueue(UUnitsProductionDataAsset* NewUnit);
	
	UFUNCTION(BlueprintCallable, Category = "Production")
	void RemoveUnitFromQueue(UUnitsProductionDataAsset* UnitToRemove);

	UFUNCTION(BlueprintCallable, Category = "Production")
	void StartUnitProduction();

	UFUNCTION(BlueprintCallable, Category = "Production")
	void StopUnitProduction();

	UFUNCTION(BlueprintCallable, Category = "Production")
	UUnitsProductionDataAsset* GetSelectedUnit();

	UPROPERTY(BlueprintAssignable, Category = "Production")
	FOnUnitProducedSignature OnUnitProduced;

	UPROPERTY(BlueprintAssignable, Category = "Production")
	FOnProductionProgress OnProductionProgress;

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
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Production")
	TArray<UUnitsProductionDataAsset*> UnitsList;
	
protected:
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Production")
	UUnitsProductionDataAsset* UnitSelected;
	
	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Production")
	TArray<UUnitsProductionDataAsset*> ProductionQueue; 

	/*-------- Prod Time --------*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnProdProgress, Category = "Production")
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

	UFUNCTION()
	void OnProdProgress();

	UFUNCTION(Server, Reliable)
	void Server_StartUnitProduction();
	
	UFUNCTION(Server, Reliable)
	void Server_StopUnitProduction();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnUnitProduced(AActor* NewUnit);

	UFUNCTION()
	void OnProductionFinished();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpdateProductionQueue(const TArray<UUnitsProductionDataAsset*>& NewQueue);

	UFUNCTION()
	FCommandData GetDestination();

	UFUNCTION()
	void OnNewSelected(bool bIsSelected);
};
