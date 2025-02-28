#include "Framwork/UI/Menu/Multiplayer/SMenuJoinSessionWidget.h"
#include "OnlineSubsystem.h"
#include "CommonBorder.h"
#include "PrimaryGameLayout.h"
#include "Framwork/UI/Menu/SButtonBaseWidget.h"
#include "CommonListView.h"
#include "FindSessionsCallbackProxyAdvanced.h"
#include "OnlineSubsystemUtils.h"
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
		JoinByID->OnClicked().AddUObject(this, &USMenuJoinSessionWidget::OnJoinByID);

	if (JoinSessionIdButton)
		JoinSessionIdButton->OnClicked().AddUObject(this, &USMenuJoinSessionWidget::OnSearchSessionById);

	if (BackSessionIdButton)
		BackSessionIdButton->OnClicked().AddUObject(this, &USMenuJoinSessionWidget::OnJoinByID);

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
		StartSearch();
}

/*--------------------------------- Search Sessions -------------------------------------*/
#pragma region Search Sessions

void USMenuJoinSessionWidget::StartSearch()
{
	    UE_LOG(LogTemp, Warning, TEXT("[USMenuJoinSessionWidget::StartSearch] - Start Search"));

    if (bSearchInProgress || !GetOwningPlayer() || !GetOwningPlayer()->GetWorld() || !ListView)
    {
        UE_LOG(LogTemp, Warning, TEXT("[USMenuJoinSessionWidget::StartSearch] - Search Failed"));
        return;
    }

    bSearchInProgress = true;
    NoSessionsDisplay->SetVisibility(ESlateVisibility::Collapsed);
    SetSpinnerDisplay(true);

    // Récupérer le PlayerController
    APlayerController* PlayerController = GetOwningPlayer();
    if (!PlayerController)
    {
        UE_LOG(LogTemp, Error, TEXT("[USMenuJoinSessionWidget::StartSearch] - Invalid PlayerController"));
        return;
    }

    // Définir les filtres de recherche si nécessaire
    TArray<FSessionsSearchSetting> SearchFilters;
    // Exemple : Ajouter un filtre pour une propriété spécifique
    // FSessionPropertyKeyPair MapFilter;
    // MapFilter.Key = FName("MapName");
    // MapFilter.Value = FString("YourMapName");
    // SearchFilters.Add(MapFilter);

    // Lancer la recherche de sessions
    UFindSessionsCallbackProxyAdvanced* FindSessionProxy = UFindSessionsCallbackProxyAdvanced::FindSessionsAdvanced(
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

    if (FindSessionProxy)
    {
        FindSessionProxy->OnSuccess.AddDynamic(this, &USMenuJoinSessionWidget::OnSessionSearchComplete);
        FindSessionProxy->OnFailure.AddDynamic(this, &USMenuJoinSessionWidget::OnSessionSearchFailed);
        FindSessionProxy->Activate();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[USMenuJoinSessionWidget::StartSearch] - Failed to create FindSessionProxy"));
        bSearchInProgress = false;
        SetSpinnerDisplay(false);
    }
}

void USMenuJoinSessionWidget::OnSessionSearchComplete(const TArray<FBlueprintSessionResult>& Results)
{
	SessionsList = Results;
	
	bSearchInProgress = false;
	SetSpinnerDisplay(false);
    
	TArray<UObject*> WrappedResults;
	for (const FBlueprintSessionResult& Result : Results)
	{
		UBlueprintSessionResultObject* Wrapper = NewObject<UBlueprintSessionResultObject>(this);
		Wrapper->SessionResult = Result;
		WrappedResults.Add(Wrapper);
	}
    
	if (WrappedResults.Num() > 0)
	{
		ListView->SetListItems(WrappedResults);
		ListView->SetUserFocus(GetOwningPlayer());
		NoSessionsDisplay->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		NoSessionsDisplay->SetVisibility(ESlateVisibility::Visible);
	}
}

void USMenuJoinSessionWidget::OnSessionSearchFailed(const TArray<FBlueprintSessionResult>& Results)
{
	NoSessionsDisplay->SetVisibility(ESlateVisibility::Visible);

	bSearchInProgress = false;
	SetSpinnerDisplay(false);
	
	UE_LOG(LogTemp, Error, TEXT("[USMenuJoinSessionWidget::OnSessionSearchComplete] - Failed Session Search"))
}
#pragma endregion

/*--------------------------------- Join Session By ID -------------------------------------*/
#pragma region Search Sessions By ID

void USMenuJoinSessionWidget::OnJoinByID()
{
	if (!SessionIdOverlay->IsVisible())
		SetSessionIdDisplay(true);
	else
		SetSessionIdDisplay(false);
}

void USMenuJoinSessionWidget::OnSearchSessionById()
{
	JoinSessionByID(SessionIdEditableText->GetText().ToString());
}

void USMenuJoinSessionWidget::SetSessionIdDisplay(bool bNewDisplay) const
{
	if (SessionIdOverlay)
		SessionIdOverlay->SetVisibility(bNewDisplay ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

// Utility ID Session
void USMenuJoinSessionWidget::JoinSessionByID(const FString& NewSessionId)
{
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld());
	if (!OnlineSubsystem)
		return;

	IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
	if (!SessionInterface)
		return;

	bool bFound = false;

	for (const FBlueprintSessionResult& Result : SessionsList)
	{
		if (!Result.OnlineResult.GetSessionIdStr().IsEmpty())
		{
			FString CurrentSessionId = Result.OnlineResult.GetSessionIdStr();
			if (CurrentSessionId == NewSessionId)
			{
				SessionById = Result;
				bFound = true;
				break;
			}
		}
	}

	if (!bFound || !SessionById.OnlineResult.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Aucune session trouvée avec l'ID : %s"), *NewSessionId);
		return;
	}
	
	OnJoinSessionIdIsFound();
}

void USMenuJoinSessionWidget::OnJoinSessionIdIsFound_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Session found"));
}

#pragma endregion
