#include "Framwork/UI/Menu/GameMenu/SButtonSessionId.h"
#include "AdvancedSessionsLibrary.h"
#include "CommonTextBlock.h"
#include "Windows/WindowsPlatformApplicationMisc.h"


void USButtonSessionId::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	OnButtonClickedDelegate.AddDynamic(this, &USButtonSessionId::OnIdClicked);
	
	FString SessionId;
	UAdvancedSessionsLibrary::GetCurrentSessionID_AsString(GetWorld(), SessionId);

	SetButtonText(FText::FromString(SessionId));
}

void USButtonSessionId::OnIdClicked()
{
	FPlatformApplicationMisc::ClipboardCopy(*ButtonTextBlock->GetText().ToString());

	if (!IsPlayingAnimation())
	{
		PlayAnimationForward(PressedAnimation);	
	}
	else
	{
		StopAnimation(PressedAnimation);
		PlayAnimationForward(PressedAnimation);
	}
}
