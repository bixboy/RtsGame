#include "Framwork/UI/Menu/Multiplayer/SGameDisplayWidget.h"
#include "Framwork/UI/Menu/Multiplayer/SButtonGameWidget.h"

void USGameDisplayWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (GameButtonWidget)
	{
		GameButtonWidget->OnClicked().AddUObject(this, &USGameDisplayWidget::OnGameSelection);
	}
}

void USGameDisplayWidget::SetGameData(const FPrimaryAssetId& Data)
{
	if (Data.IsValid() && GameButtonWidget)
	{
		GameDataId = Data;
		GameButtonWidget->SetGameData(Data);
	}
}

void USGameDisplayWidget::OnGameSelection()
{
	OnGameDisplaySelected.Broadcast(GameDataId);
}
