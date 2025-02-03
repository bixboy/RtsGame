﻿#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerControllerRts.generated.h"

class USelectionComponent;
class UInputMappingContext;


UCLASS()
class RTSMODE_API APlayerControllerRts : public APlayerController
{
	GENERATED_BODY()

public:
	APlayerControllerRts(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USelectionComponent* SelectionComponent;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;
	
};
