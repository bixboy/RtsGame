#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuilderComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RTSGAME_API UBuilderComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBuilderComponent();

protected:
	virtual void BeginPlay() override;
};
