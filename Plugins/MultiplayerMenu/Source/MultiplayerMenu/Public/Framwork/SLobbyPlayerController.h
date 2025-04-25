#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SLobbyPlayerController.generated.h"

class USChatComponent;
class ASLobbyPlayerState;
class USMenuLobbyWidget;


USTRUCT(BlueprintType)
struct FPlayerInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "PlayerInfo")
	ASLobbyPlayerState* PlayerState;

	UPROPERTY(BlueprintReadWrite, Category = "PlayerInfo")
	FString PlayerName;

	UPROPERTY(BlueprintReadWrite, Category = "PlayerInfo")
	bool bIsReady;

	bool operator==(const FPlayerInfo& Other) const
	{
		return (PlayerName == Other.PlayerName) && (PlayerState == Other.PlayerState);
	}

};

UCLASS(Abstract)
class MULTIPLAYERMENU_API ASLobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASLobbyPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_SetPlayerReady();

	UFUNCTION(Client, Reliable)
	void Client_UpdateWidget(const TArray<FPlayerInfo>& PlayersInfo);

	UFUNCTION()
	FPlayerInfo GetPlayerInfo();

	UFUNCTION()
	USChatComponent* GetChatComponent();

protected:
	UFUNCTION()
	void SetupWidget();

	UFUNCTION(Server, Reliable)
	void Server_SetPlayerInfo();

	UPROPERTY()
	FPlayerInfo PlayerInfo;

	UPROPERTY()
	USMenuLobbyWidget* LobbyWidget;

	UPROPERTY(EditAnywhere, Category = "Settings|Widget")
	TSubclassOf<USMenuLobbyWidget> LobbyWidgetClass;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	USChatComponent* ChatComponent;
};
