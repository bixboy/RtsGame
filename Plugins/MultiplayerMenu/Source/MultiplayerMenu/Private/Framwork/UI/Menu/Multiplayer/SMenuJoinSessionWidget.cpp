#include "Framwork/UI/Menu/Multiplayer/SMenuJoinSessionWidget.h"

#include "AdvancedSessionsLibrary.h"
#include "OnlineSubsystem.h"
#include "CommonBorder.h"
#include "Framwork/UI/Menu/SButtonBaseWidget.h"
#include "CommonListView.h"
#include "FindSessionsCallbackProxyAdvanced.h"
#include "OnlineSubsystemUtils.h"
#include "Components/BackgroundBlur.h"
#include "Components/EditableText.h"
#include "Components/EditableTextBox.h"
#include "Components/Overlay.h"
#include "Framwork/Data/BlueprintSessionResultObject.h"


/*--------------------------------- Setup -------------------------------------*/
#pragma region Setup
void USMenuJoinSessionWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (RefreshButton)
		RefreshButton->OnClicked().AddUObject(this, &USMenuJoinSessionWidget::OnRefreshList);

	if (JoinByID)
		JoinByID->OnClicked().AddUObject(this, &USMenuJoinSessionWidget::ShowJoinByID);

	if (JoinSessionIdButton)
		JoinSessionIdButton->OnClicked().AddUObject(this, &USMenuJoinSessionWidget::OnStartSearchSessionById);

	if (BackSessionIdButton)
		BackSessionIdButton->OnClicked().AddUObject(this, &USMenuJoinSessionWidget::ShowJoinByID);

	SetSessionIdDisplay(false);
	SetSpinnerDisplay(false);
}

void USMenuJoinSessionWidget::SetActivated(const bool bActivate)
{
	if (bActivate)
	{
		SetVisibility(ESlateVisibility::Visible);
		StartSearch();
	}
	else
		SetVisibility(ESlateVisibility::Collapsed);
}

UWidget* USMenuJoinSessionWidget::NativeGetDesiredFocusTarget() const
{
	if (ListView)
		return ListView;
	
	return Super::NativeGetDesiredFocusTarget();
}
#pragma endregion

void USMenuJoinSessionWidget::SetSpinnerDisplay(const bool bSpinnerState) const
{
	if (SpinnerDisplay)
		SpinnerDisplay->SetVisibility(bSpinnerState ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void USMenuJoinSessionWidget::OnRefreshList()
{
	if (!bSearchInProgress)
	{
		StartSearch();
		bSessionIdSearch = false;
	}
}

/*--------------------------------- Search Sessions -------------------------------------*/
#pragma region Search Sessions

void USMenuJoinSessionWidget::StartSearch()
{
	UE_LOG(LogTemp, Warning, TEXT("[USMenuJoinSessionWidget::StartSearch] - Start Search"));

	if (!GetOwningPlayer() || !GetOwningPlayer()->GetWorld() || !ListView) return;

	if (!bSessionIdSearch)
	{
		bSearchInProgress = true;
		NoSessionsDisplay->SetVisibility(ESlateVisibility::Collapsed);

		ListView->ClearListItems();
		SetSpinnerDisplay(true);	
	}

	APlayerController* PlayerController = GetOwningPlayer();
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("[USMenuJoinSessionWidget::StartSearch] - PlayerController invalid"));
		
		bSearchInProgress = false;
		SetSpinnerDisplay(false);
		return;
	}

	if (!bSearchInProgress || !bSessionIdSearch)
	{
		// Définir les filtres de recherche si nécessaire
		TArray<FSessionsSearchSetting> SearchFilters;
		
		// Filtre pour exclure les sessions privées
		FSessionPropertyKeyPair AccessFilter;
		AccessFilter.Key = FName("SessionAccess");
		AccessFilter.Data = 0;
		FSessionsSearchSetting FilterSetting;
		FilterSetting.PropertyKeyPair = AccessFilter;
		FilterSetting.ComparisonOp = EOnlineComparisonOpRedux::Equals;
		//SearchFilters.Add(FilterSetting);

		// Lancer la recherche de sessions
		FindSessionProxy = UFindSessionsCallbackProxyAdvanced::FindSessionsAdvanced(
			GetWorld(),
			PlayerController,
			100,				 
			false,				 
			EBPServerPresenceSearchType::AllServers,				 
			SearchFilters,      
			false,				 
			false,				 
			false,				
			true,
			0
		);	
	}

    if (FindSessionProxy)
    {
    	FindSessionProxy->OnSuccess.Clear();
    	FindSessionProxy->OnFailure.Clear();
    	FindSessionProxy->OnFailure.AddDynamic(this, &USMenuJoinSessionWidget::OnSessionSearchFailed);
    	
	    if (!bSessionIdSearch && bSearchInProgress)
	    {
	    	UE_LOG(LogTemp, Warning, TEXT("[USMenuJoinSessionWidget::StartSearch] - Bind to Search"));
	    	FindSessionProxy->OnSuccess.AddDynamic(this, &USMenuJoinSessionWidget::OnSessionSearchComplete);
	    	FindSessionProxy->Activate();   
	    }
    	else
	    {
    		UE_LOG(LogTemp, Warning, TEXT("[USMenuJoinSessionWidget::StartSearch] - Bind to ID"));
    		FindSessionProxy->OnSuccess.AddDynamic(this, &USMenuJoinSessionWidget::JoinSessionByID);
    		FindSessionProxy->Activate();   
	    }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[USMenuJoinSessionWidget::StartSearch] - Failed to create FindSessionProxy"));
        bSearchInProgress = false;
        SetSpinnerDisplay(false);
    }
}

//Search Session Callback
void USMenuJoinSessionWidget::OnSessionSearchComplete(const TArray<FBlueprintSessionResult>& Results)
{
	SessionsList = Results;
	bSearchInProgress = false;
	SetSpinnerDisplay(false);
    
	TArray<UObject*> WrappedResults;
	for (const FBlueprintSessionResult& Result : Results)
	{
		bool bIsPrivate = false;

		TArray<FSessionPropertyKeyPair> ExtraSettings;
		UAdvancedSessionsLibrary::GetExtraSettings(Result, ExtraSettings);
		for (const FSessionPropertyKeyPair& Setting : ExtraSettings)
		{
			if (Setting.Key == "GameName")
			{
				FString GameName;
				Setting.Data.GetValue(GameName);
				if (GameName.IsEmpty())
				{
					bIsPrivate = true;
					break;	
				}
			}
			
			if (Setting.Key == "SessionAccess")
			{
				int32 SessionAccessValue;
				Setting.Data.GetValue(SessionAccessValue);
				bIsPrivate = (SessionAccessValue != 0);
				break;
			}
		}

		// Ajouter à la liste uniquement si la session n'est pas privée
		if (!bIsPrivate)
		{
			UBlueprintSessionResultObject* Wrapper = NewObject<UBlueprintSessionResultObject>(this);
			Wrapper->SessionResult = Result;
			WrappedResults.Add(Wrapper);
		}
	}
    
	if (WrappedResults.Num() > 0)
	{
		ListView->SetListItems(WrappedResults);
		ListView->SetUserFocus(GetOwningPlayer());
		NoSessionsDisplay->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		OnSessionSearchFailed(Results);
		NoSessionsDisplay->SetVisibility(ESlateVisibility::Visible);
	}

	UE_LOG(LogTemp, Warning, TEXT("[USMenuJoinSessionWidget::OnSessionSearchComplete] - Success Session Search"))
}

void USMenuJoinSessionWidget::OnSessionSearchFailed(const TArray<FBlueprintSessionResult>& Results)
{
	NoSessionsDisplay->SetVisibility(ESlateVisibility::Visible);
	SessionIdBlur->SetVisibility(ESlateVisibility::Collapsed);

	bSearchInProgress = false;
	bSessionIdSearch = false;
	SessionId = "None";
	
	SetSpinnerDisplay(false);
	
	UE_LOG(LogTemp, Error, TEXT("[USMenuJoinSessionWidget::OnSessionSearchComplete] - Failed Session Search"))
}

#pragma endregion

/*--------------------------------- Join Session By ID -------------------------------------*/
#pragma region Search Sessions By ID

void USMenuJoinSessionWidget::ShowJoinByID()
{
	if (bSessionIdSearch) return; 
	
	if (!SessionIdOverlay->IsVisible())
		SetSessionIdDisplay(true);
	else
		SetSessionIdDisplay(false);
}

void USMenuJoinSessionWidget::SetSessionIdDisplay(bool bNewDisplay) const
{
	if (bSessionIdSearch) return;
	
	if (SessionIdOverlay)
		SessionIdOverlay->SetVisibility(bNewDisplay ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void USMenuJoinSessionWidget::OnStartSearchSessionById()
{
	if (bSessionIdSearch) return;
	
	SessionId = SessionIdEditableText->GetText().ToString();
	
	SessionIdBlur->SetVisibility(ESlateVisibility::Visible);
	bSessionIdSearch = true;

	if (!bSearchInProgress)
	{
		StartSearch();	
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[USMenuJoinSessionWidget::OnSearchSessionById] - Change Bind to ID"));
		
		FindSessionProxy->OnSuccess.Clear();
		FindSessionProxy->OnSuccess.AddDynamic(this, &USMenuJoinSessionWidget::JoinSessionByID);	
	}
}

// Utility ID Session
void USMenuJoinSessionWidget::JoinSessionByID(const TArray<FBlueprintSessionResult>& Results)
{
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld());
	if (!OnlineSubsystem)
		return;

	IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
	if (!SessionInterface)
		return;

	bool bFound = false;

	for (const FBlueprintSessionResult& Result : Results)
	{
		if (!Result.OnlineResult.GetSessionIdStr().IsEmpty())
		{
			FString CurrentSessionId = Result.OnlineResult.GetSessionIdStr();
			if (CurrentSessionId == SessionId)
			{
				SessionById = Result;
				bFound = true;
				break;
			}
		}
	}

	if (!bFound || !SessionById.OnlineResult.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[USMenuJoinSessionWidget::JoinSessionByID] - Aucune session trouvée avec l'ID : %s"), *SessionId);
		
		SessionIdBlur->SetVisibility(ESlateVisibility::Collapsed);
		SetSpinnerDisplay(false);

		bSearchInProgress = false;
		bSessionIdSearch = false;
		
		return;
	}
	bSessionIdSearch = false;
	
	OnJoinSessionIdIsFound();
}

void USMenuJoinSessionWidget::OnJoinSessionIdIsFound_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Session found"));
}

#pragma endregion
