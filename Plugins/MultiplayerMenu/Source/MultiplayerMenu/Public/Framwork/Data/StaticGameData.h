#pragma once

#define S_MAT_PARAM_NAME_TEXTURE			FName(TEXT("Texture"))
#define S_MAT_PARAM_NAME_HOVER				FName(TEXT("Hover_Animate"))

#define S_MAT_PARAM_NAME_TEXT_ALPHA			FName(TEXT("TextAlpha"))
#define S_MAT_PARAM_NAME_TEXT_ALPHA_HOVER	FName(TEXT("TextAlphaHover"))

#define S_MAT_PARAM_NAME_TEXT_SCALE			FName(TEXT("TextScale"))
#define S_MAT_PARAM_NAME_TEXT_SCALE_HOVER	FName(TEXT("TextScaleHover"))

#define S_MAT_PARAM_NAME_TEXT_SHIFTX		FName(TEXT("Shift_X"))
#define S_MAT_PARAM_NAME_TEXT_SHIFTY		FName(TEXT("Shift_Y"))

// Asset manager data types
#define S_DATA_ASSET_TYPE_GAMEDATA			FName(TEXT("GameData"))
#define S_DATA_ASSET_TYPE_GAMEDATA_DEFAULT	FName(TEXT("DA_GameData_Default"))

// Multiplayer settings
#define S_MP_SETTINGS_GAMEMODE				TEXT("GameMode")

// Asset Bundles
#define S_DATA_ASSET_BUNDLE_GAME			FName(TEXT("Game"))
#include "NativeGameplayTags.h"

namespace UILayerTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_UI_LAYER_GAME);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_UI_LAYER_GAMEMENU);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_UI_LAYER_MENU);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_UI_LAYER_MODAL);
}
