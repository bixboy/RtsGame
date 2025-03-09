#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SChatComponent.generated.h"

struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
class USChatWidget;
class USChatBoxWidget;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MULTIPLAYERMENU_API USChatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
	void SetupInputComponent();

	UPROPERTY(EditAnywhere, Category = "Settings|Chat")
	TSubclassOf<USChatWidget> ChatWidgetClass;

	UFUNCTION(Server, Reliable)
	void Server_SendMessage(const FString& NewMessage);

	UFUNCTION(Client, Reliable)
	void Client_AddMessage(const FString& NewMessage);
	
protected:
	UPROPERTY()
	USChatWidget* ChatWidget;

	UPROPERTY()
	USChatBoxWidget* ChatBox;

	UPROPERTY(EditAnywhere, Category = "Settings|Chat")
	UInputMappingContext* MappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Inputs", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> ScrollAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Inputs", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> EnterAction;

	UFUNCTION()
	void OnScrolling(const FInputActionValue& InputActionValue);

	UFUNCTION()
	void OnEnterPressed();
};
