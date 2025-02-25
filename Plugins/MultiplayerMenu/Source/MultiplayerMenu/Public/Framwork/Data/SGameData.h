#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SGameData.generated.h"

class UCommonUserWidget;
class UCommonActivatableWidget;


UCLASS()
class MULTIPLAYERMENU_API USGameData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Asset Settings")
	FPrimaryAssetType DataType;

	virtual  FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(DataType, GetFName());
	}

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=GameData, meta=(AllowedTypes = "Map"))
	FPrimaryAssetId MapID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= GameData)
	TMap<FString, FString> ExtraArgs;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= GameData)
	int32 MaxPlayerCount;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= GameData)
	FText GameMap;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= GameData)
	FText GameName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= GameData)
	FText GameDesc;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= GameData)
	TSoftObjectPtr<UTexture2D> GameIcon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= GameData)
	bool bShowMenu = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= GameData)
	bool bIncludeGameList = true;

	UPROPERTY(EditAnywhere, Category= UI)
	TSoftClassPtr<UCommonActivatableWidget> MainMenuClass;

	UPROPERTY(EditAnywhere, Category= UI)
	TSoftClassPtr<UCommonUserWidget> GameDisplayWidgetClass;
};
