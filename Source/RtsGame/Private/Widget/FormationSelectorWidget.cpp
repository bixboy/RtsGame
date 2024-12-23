#include "Widget/FormationSelectorWidget.h"

#include "PlayerControllerRts.h"
#include "Components/Slider.h"
#include "Kismet/GameplayStatics.h"
#include "Widget/FormationButtonWidget.h"

void UFormationSelectorWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	verify((PlayerController = Cast<APlayerControllerRts>(UGameplayStatics::GetPlayerController(GetWorld(), 0))) != nullptr);

	if (LineButton && ColumnButton && WedgeButton && BlobButton)
	{
		OnFormationButtonClicked(LineButton, 0);
		LineButton->OnButtonClicked.AddDynamic(this, &UFormationSelectorWidget::OnFormationButtonClicked);
		ColumnButton->OnButtonClicked.AddDynamic(this, &UFormationSelectorWidget::OnFormationButtonClicked);
		WedgeButton->OnButtonClicked.AddDynamic(this, &UFormationSelectorWidget::OnFormationButtonClicked);
		BlobButton->OnButtonClicked.AddDynamic(this, &UFormationSelectorWidget::OnFormationButtonClicked);
	}

	if (SpacingSlider)
	{
		SpacingSlider->OnValueChanged.AddDynamic(this, &UFormationSelectorWidget::OnSpacingSliderValueChanged);
		OnSpacingSliderValueChanged(SpacingSlider->GetValue());
	}
}

void UFormationSelectorWidget::OnFormationButtonClicked(UCustomButton* Button, int Index)
{
	if (PlayerController)
	{
		PlayerController->UpdateFormation(static_cast<EFormation>(Index));
	}
}

void UFormationSelectorWidget::OnSpacingSliderValueChanged(const float Value)
{
	if (PlayerController)
	{
		PlayerController->UpdateSpacing(Value);
	}
}
