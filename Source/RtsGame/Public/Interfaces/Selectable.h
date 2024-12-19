#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Selectable.generated.h"

UINTERFACE()
class USelectable : public UInterface
{
	GENERATED_BODY()
};

class RTSGAME_API ISelectable
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual void Select() = 0;
	UFUNCTION()
	virtual void Deselect() = 0;

	UFUNCTION()
	virtual void Highlight(const bool Highlight) = 0;
};
