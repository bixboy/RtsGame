#include "Framwork/UI/SPrimaryGameLayout.h"

#include "Framwork/Data/StaticGameData.h"


void USPrimaryGameLayout::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	RegisterLayer(UILayerTags::TAG_UI_LAYER_GAME, Game_Stack);
	RegisterLayer(UILayerTags::TAG_UI_LAYER_GAME, GameMenu_Stack);
	RegisterLayer(UILayerTags::TAG_UI_LAYER_GAME, Menu_Stack);
	RegisterLayer(UILayerTags::TAG_UI_LAYER_GAME, Modal_Stack);
}
