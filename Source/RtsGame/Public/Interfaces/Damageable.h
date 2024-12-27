#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Damageable.generated.h"

UINTERFACE()
class UDamageable : public UInterface
{
	GENERATED_BODY()
};


class RTSGAME_API IDamageable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void TakeDamage(AActor* DamageOwner);
};
