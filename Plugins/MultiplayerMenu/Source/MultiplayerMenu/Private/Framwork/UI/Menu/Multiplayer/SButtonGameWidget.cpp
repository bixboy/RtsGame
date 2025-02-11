#include "Framwork/UI/Menu/Multiplayer/SButtonGameWidget.h"

#include "Engine/AssetManager.h"
#include "Framwork/Data/SGameData.h"

void USButtonGameWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void USButtonGameWidget::SetGameData(const FPrimaryAssetId& GameDataAssetId)
{
	if (GameDataAssetId.IsValid())
	{
		if (const UAssetManager* AssetManager = UAssetManager::GetIfInitialized())
		{
			if (const USGameData* GameData = Cast<USGameData>(AssetManager->GetPrimaryAssetObject(GameDataAssetId)))
			{
				InitButton();
				UpdateButtonText(GameData->GameMap, GameData->GameName, GameData->GameDesc);
				if (GameData->GameIcon.LoadSynchronous())
				{
					ButtonTexture = GameData->GameIcon.LoadSynchronous();
					SetButtonSettings();
				}
			}
		}		
	}
}

void USButtonGameWidget::UpdateButtonText(const FText& GameMapText, const FText& GameNameText,
	const FText& GameDescText)
{
}
