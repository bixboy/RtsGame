#pragma once
#include "CoreMinimal.h"
#include "Components/RtsResourcesComponent.h"
#include "Data/DataRts.h"
#include "GameFramework/Actor.h"
#include "ResourceNode.generated.h"

class UBoxComponent;
class URtsResourcesComponent;

UCLASS()
class RTSGAME_API AResourceNode : public AActor, public ISelectable
{
	GENERATED_BODY()

public:
	AResourceNode();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnResourceUpdated(const FResourcesCost& NewResources);

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	/*---- Variables ----*/
	UFUNCTION()
	FResourcesCost StartResourceCollect(int TakeNumber);

	UFUNCTION()
	bool GetIsEmpty(EResourceType CheckResource = EResourceType::None);

	UFUNCTION()
	EResourceType GetResourceType();
	
protected:	
	/*---- Variables ----*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UBoxComponent* BoxComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* StaticMeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	URtsResourcesComponent* ResourcesComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	EResourceType ResourceType = EResourceType::Wood;

	
// ----------------------- Interfaces -----------------------
#pragma region Interfaces
	/*- Function -*/
	virtual void Select() override;
	virtual void Deselect() override;
	virtual void Highlight(const bool Highlight) override;

protected:
	/*- Variables -*/
	UPROPERTY()
	bool Selected;

#pragma endregion	
};
