#include "Widget/HudWidget.h"

#include "Player/PlayerControllerRts.h"
#include "Components/Button.h"
#include "Components/SlectionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Widget/Formations/FormationSelectorWidget.h"
#include "Widget/Behaviors/SelectBehaviorWidget.h"

void UHudWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	verify((PlayerController = Cast<APlayerControllerRts>(UGameplayStatics::GetPlayerController(GetWorld(), 0))) != nullptr);

	SetFormationSelectionWidget(false);
	SetBehaviorSelectionWidget(false);
	SetUnitsSelectionWidget(true);

	if (PlayerController && PlayerController->SelectionComponent)
	{
		SelectionComponent = PlayerController->SelectionComponent;
		SelectionComponent->OnSelectedUpdate.AddDynamic(this, &UHudWidget::OnSelectionUpdated);
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

void UHudWidget::SetUnitsSelectionWidget(bool bEnabled) const
{
	if (UnitsSelector)
	{
		BehaviorSelector->SetVisibility(bEnabled ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UHudWidget::OnSelectionUpdated()
{
	if(SelectionComponent)
	{
		SetFormationSelectionWidget(SelectionComponent->HasGroupSelection());
		if (!SelectionComponent->GetSelectedActors().IsEmpty())
		{
			SetBehaviorSelectionWidget(true);
		}
		else
		{
			SetBehaviorSelectionWidget(false);
		}
	}
}
