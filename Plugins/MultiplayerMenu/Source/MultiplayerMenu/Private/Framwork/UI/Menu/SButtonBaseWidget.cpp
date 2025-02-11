#include "Framwork/UI/Menu/SButtonBaseWidget.h"
#include "CommonTextBlock.h"
#include "CommonBorder.h"
#include "Framwork/Data/StaticGameData.h"

void USButtonBaseWidget::SetButtonBlock(const FText& InText)
{
	bOverride_ButtonText = InText.IsEmpty();
	ButtonText = InText;
	UpdateButtonText(ButtonText);
}

void USButtonBaseWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	InitButton();
}

void USButtonBaseWidget::InitButton()
{
	OnHovered().AddUObject(this, &USButtonBaseWidget::OnButtonHovered);
	OnUnhovered().AddUObject(this, &USButtonBaseWidget::OnButtonUnHovered);

	ApplyMaterial();
	UpdateButtonText(ButtonText);
	SetButtonSettings();
}

void USButtonBaseWidget::OnButtonHovered()
{
	if(ButtonBorder)
	{
		if(UMaterialInstanceDynamic* MaterialInstance = ButtonBorder->GetDynamicMaterial())
			MaterialInstance->SetScalarParameterValue("", 1.f);
	}
}

void USButtonBaseWidget::OnButtonUnHovered()
{
	if(ButtonBorder)
	{
		if(UMaterialInstanceDynamic* MaterialInstance = ButtonBorder->GetDynamicMaterial())
			MaterialInstance->SetScalarParameterValue("", 0.f);
	}
}

void USButtonBaseWidget::ApplyMaterial() const
{
	if (ButtonBorder && Material)
	{
		ButtonBorder->SetBrushFromMaterial(Material);
	}
}

void USButtonBaseWidget::UpdateButtonText(const FText& InText)
{
	if (ButtonTextBlock)
	{
		ButtonTextBlock->SetText(InText);
	}
}

void USButtonBaseWidget::SetButtonSettings() const
{
	if (ButtonBorder)
	{
		if (UMaterialInstanceDynamic* MaterialInstance = ButtonBorder->GetDynamicMaterial())
		{
			if (bOverride_Texture_Alpha > 0)
			{
				MaterialInstance->SetScalarParameterValue(S_MAT_PARAM_NAME_TEXT_ALPHA, TextureAlpha);
				MaterialInstance->SetScalarParameterValue(S_MAT_PARAM_NAME_TEXT_ALPHA_HOVER, TextureHoverAlpha);
			}

			if (bOverride_Texture_Scale > 0)
			{
				MaterialInstance->SetScalarParameterValue(S_MAT_PARAM_NAME_TEXT_SCALE, TextureScale);
				MaterialInstance->SetScalarParameterValue(S_MAT_PARAM_NAME_TEXT_SCALE_HOVER, TextureHoverScale);
			}

			if (bOverride_Texture_Shift > 0)
			{
				MaterialInstance->SetScalarParameterValue(S_MAT_PARAM_NAME_TEXT_SHIFTX, TextureShiftX);
				MaterialInstance->SetScalarParameterValue(S_MAT_PARAM_NAME_TEXT_SHIFTY, TextureShiftY);
			}

			if (ButtonTexture)
			{
				MaterialInstance->SetTextureParameterValue(S_MAT_PARAM_NAME_TEXTURE, ButtonTexture);
			}
		}
	}
}
