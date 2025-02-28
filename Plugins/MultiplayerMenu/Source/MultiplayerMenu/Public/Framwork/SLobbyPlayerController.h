#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SLobbyPlayerController.generated.h"

class USMenuLobbyWidget;

USTRUCT(BlueprintType)
struct FPlayerInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "PlayerInfo")
	APlayerController* PlayerController;

	UPROPERTY(BlueprintReadWrite, Category = "PlayerInfo")
	FString PlayerName;

	UPROPERTY(BlueprintReadWrite, Category = "PlayerInfo")
	int32 PlayerId;

	UPROPERTY(BlueprintReadWrite, Category = "PlayerInfo")
	bool bIsReady;

};

UCLASS(Abstract)
class MULTIPLAYERMENU_API ASLobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASLobbyPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnPlayerJoinSession(APlayerController* NewController);

	UPROPERTY()
	FPlayerInfo PlayerInfo;

protected:
	UFUNCTION()
	void SetupWidget();

	UFUNCTION()
	void RefreshPlayerList();

	UPROPERTY()
	USMenuLobbyWidget* LobbyWidget;

	UPROPERTY(EditAnywhere, Category = "Settings|Widget")
	TSubclassOf<USMenuLobbyWidget> LobbyWidgetClass;
};
