#include "Widget/HudWidget.h"

#include "PlayerControllerRts.h"
#include "Kismet/GameplayStatics.h"
#include "Widget/FormationSelectorWidget.h"

void UHudWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	verify((PlayerController = Cast<APlayerControllerRts>(UGameplayStatics::GetPlayerController(GetWorld(), 0))) != nullptr);

	SetFormationSelectionWidget(false);

	if (PlayerController)
	{
		PlayerController->OnSelectedUpdate.AddDynamic(this, &UHudWidget::OnSelectionUpdated);
	}
}

void UHudWidget::SetFormationSelectionWidget(const bool bEnabled) const
{
	if (FormationSelector)
	{
		FormationSelector->SetVisibility(bEnabled ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void UHudWidget::OnSelectionUpdated()
{
	if(PlayerController)
	{
		SetFormationSelectionWidget(PlayerController->HasGroupSelection());
	}
}
