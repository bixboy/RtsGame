#include "Widget/HudWidget.h"

#include "PlayerControllerRts.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Widget/FormationSelectorWidget.h"
#include "Widget/SelectBehaviorWidget.h"

void UHudWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	verify((PlayerController = Cast<APlayerControllerRts>(UGameplayStatics::GetPlayerController(GetWorld(), 0))) != nullptr);

	SetFormationSelectionWidget(false);
	SetBehaviorSelectionWidget(false);

	if (PlayerController)
	{
		PlayerController->OnSelectedUpdate.AddDynamic(this, &UHudWidget::OnSelectionUpdated);
	}
}

void UHudWidget::SetFormationSelectionWidget(const bool bEnabled) const
{
	if (FormationSelector)
	{
		FormationSelector->SetVisibility(bEnabled ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		Btn_SwitchFormation->SetVisibility(bEnabled ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UHudWidget::SetBehaviorSelectionWidget(const bool bEnabled) const
{
	if (BehaviorSelector)
	{
		BehaviorSelector->SetVisibility(bEnabled ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		Btn_SwitchBehavior->SetVisibility(bEnabled ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UHudWidget::OnSelectionUpdated()
{
	if(PlayerController)
	{
		SetFormationSelectionWidget(PlayerController->HasGroupSelection());
		if (!PlayerController->GetSelectedActors().IsEmpty())
		{
			SetBehaviorSelectionWidget(true);
		}
		else
		{
			SetBehaviorSelectionWidget(false);
		}
	}
}
