#include "Component/ProximityPromptManagerComponent.h"
#include "Component/ProximityPromptComponent.h"


UProximityPromptManagerComponent::UProximityPromptManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UProximityPromptManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	
	Player = Cast<APawn>(GetOwner());
	
	if (Player)
		PC = Cast<APlayerController>(Player->GetController());
}


void UProximityPromptManagerComponent::RegisterPrompt(UProximityPromptComponent* Prompt)
{
	
	if (!Prompt || AllPrompts.Contains(Prompt) || !PC)
		return;
	
	AllPrompts.Add(Prompt);
	Server_RegisterPrompt(Prompt);

	UInputAction* IA = Prompt->InteractInputAction;
	if (!IA || BoundActions.Contains(IA))
		return;

	if (UEnhancedInputComponent* EI = Cast<UEnhancedInputComponent>(PC->InputComponent))
	{
		EI->BindAction(IA, ETriggerEvent::Started, this, &UProximityPromptManagerComponent::OnAnyPromptAction);
		BoundActions.Add(IA);
	}
}

void UProximityPromptManagerComponent::Server_RegisterPrompt_Implementation(UProximityPromptComponent* Prompt)
{
	if (!Prompt || AllPrompts.Contains(Prompt))
		return;

	AllPrompts.Add(Prompt);
}

void UProximityPromptManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!GetOwner()->HasAuthority() || !Player || !PC)
		return;

	const FVector PlayerPos  = Player->GetActorLocation();

	static FVector2D ScreenCenter = [](){
		FVector2D VS; GEngine->GameViewport->GetViewportSize(VS);
		return VS * 0.5f;
	}();

	UProximityPromptComponent* BestPrompt = nullptr;
	float BestDist2  = FLT_MAX;

	for (UProximityPromptComponent* P : AllPrompts)
	{
		if (!P || !P->IsPromptEnabled()) 
			continue;

		float D2 = FVector::DistSquared(PlayerPos, P->GetPromptLocation());
		float R2 = FMath::Square(P->GetActivationDistance());
		if (D2 > R2) 
			continue;

		FVector2D ScreenPos;
		if (!PC->ProjectWorldLocationToScreen(P->GetPromptLocation(), ScreenPos))
			continue;

		float dx = ScreenPos.X - ScreenCenter.X;
		float dy = ScreenPos.Y - ScreenCenter.Y;
		float Dist2 = dx*dx + dy*dy;

		if (Dist2 < BestDist2)
		{
			BestDist2  = Dist2;
			BestPrompt = P;
		}
	}

	if (BestPrompt)
	{
		AActor* Owner = BestPrompt->GetOwner();
		TArray<UProximityPromptComponent*> Prompts;
		
		Owner->GetComponents<UProximityPromptComponent>(Prompts);
		const int32 PromptIndex = Prompts.IndexOfByKey(BestPrompt);

		if (PromptIndex != INDEX_NONE)
		{
			Client_SetPrompt(Owner, PromptIndex);
		}
	}
	else
	{
		Client_SetPrompt(nullptr, -1);
	}
}

void UProximityPromptManagerComponent::Client_SetPrompt_Implementation(AActor* PromptOwner, int32 PromptIndex)
{
	UProximityPromptComponent* NewPrompt = nullptr;

	if (PromptOwner && PromptIndex >= 0)
	{
		TArray<UProximityPromptComponent*> Prompts;
		PromptOwner->GetComponents<UProximityPromptComponent>(Prompts);

		if (Prompts.IsValidIndex(PromptIndex))
		{
			NewPrompt = Prompts[PromptIndex];
		}
	}

	if (CurrentPrompt.Get() == NewPrompt)
		return;

	if (CurrentPrompt.IsValid())
		CurrentPrompt->SetPromptVisible(false);

	if (NewPrompt)
	{
		NewPrompt->SetPromptVisible(true);
		CurrentPrompt = NewPrompt;
	}
	else
	{
		CurrentPrompt = nullptr;
	}
}


void UProximityPromptManagerComponent::OnAnyPromptAction(const FInputActionInstance& Instance)
{
	
	const UInputAction* Fired = Instance.GetSourceAction();
	if (!Fired)
		return;

	if (CurrentPrompt.IsValid() && CurrentPrompt->InteractInputAction == Fired)
	{
		if (CurrentPrompt->IsPromptEnabled() && CurrentPrompt->IsPromptVisible())
			OnPromptInteracted.Broadcast( CurrentPrompt.Get(), PC, CurrentPrompt->GetOwner() );
	}
}

void UProximityPromptManagerComponent::SetAllPromptsEnabled(bool bNewEnabled)
{
	for (const TWeakObjectPtr<UProximityPromptComponent>& Prompt : AllPrompts)
	{
		if (Prompt.IsValid() && Prompt->IsPromptEnabled() != bNewEnabled)
		{
			Prompt->SetPromptEnabled(bNewEnabled);
		}
	}
}

