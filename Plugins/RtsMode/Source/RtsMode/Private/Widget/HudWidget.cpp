﻿#include "Widget/HudWidget.h"
#include "Player/PlayerControllerRts.h"
#include "Components/Button.h"
#include "Components/SlectionComponent.h"
#include "Interfaces/Selectable.h"
#include "Kismet/GameplayStatics.h"
#include "Widget/Formations/FormationSelectorWidget.h"
#include "Widget/Behaviors/SelectBehaviorWidget.h"
#include "Widget/UnitsSelection/UnitsSelectionWidget.h"


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
		UnitsSelector->SetVisibility(bEnabled ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UHudWidget::OnSelectionUpdated(TArray<AActor*> NewSelection)
{
	if(SelectionComponent)
	{
		TArray<AActor*> Selection = SelectionComponent->GetSelectedActors();
		if (Selection.IsEmpty())
		{
			SetBehaviorSelectionWidget(false);
			SetFormationSelectionWidget(false);
			return;
		}

		if (ISelectable::Execute_GetSelectionType(Selection[0]) == ESelectionType::Unit)
		{
			SetFormationSelectionWidget(SelectionComponent->HasGroupSelection());
			SetBehaviorSelectionWidget(true);	
		}
	}
}
