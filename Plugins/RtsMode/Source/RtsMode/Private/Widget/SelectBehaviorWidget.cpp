#include "Widget/SelectBehaviorWidget.h"

#include "PlayerControllerRts.h"
#include "Kismet/GameplayStatics.h"
#include "Widget/BehaviorButtonWidget.h"

void USelectBehaviorWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	verify((PlayerController = Cast<APlayerControllerRts>(UGameplayStatics::GetPlayerController(GetWorld(), 0))) != nullptr);

	if (NeutralButton && PassiveButton && AggressiveButton)
	{
		OnBehaviorButtonClicked(NeutralButton, 0);
		NeutralButton->OnButtonClicked.AddDynamic(this, &USelectBehaviorWidget::OnBehaviorButtonClicked);
		PassiveButton->OnButtonClicked.AddDynamic(this, &USelectBehaviorWidget::OnBehaviorButtonClicked);
		AggressiveButton->OnButtonClicked.AddDynamic(this, &USelectBehaviorWidget::OnBehaviorButtonClicked);
	}
}

void USelectBehaviorWidget::OnBehaviorButtonClicked(UCustomButton* Button, int Index)
{
	if (PlayerController)
	{
		PlayerController->UpdateBehavior(static_cast<ECombatBehavior>(Index));
	}
}

