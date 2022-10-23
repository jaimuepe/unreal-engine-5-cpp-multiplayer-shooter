// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"

#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem() :
	CREATE_DELEGATE(CreateSessionComplete),
	CREATE_DELEGATE(FindSessionsComplete),
	CREATE_DELEGATE(JoinSessionComplete),
	CREATE_DELEGATE(DestroySessionComplete),
	CREATE_DELEGATE(StartSessionComplete)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		SessionInterface = Subsystem->GetSessionInterface();
	}
}

void UMultiplayerSessionsSubsystem::CreateSession(int32 NumPublicConnections, FString NameOfSession, FString MatchType)
{
	if (!SessionInterface.IsValid())
	{
		MultiplayerOnCreateSessionComplete.Broadcast(false);
		return;
	}

	FName Name(NameOfSession);

	FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(Name);
	if (ExistingSession)
	{
		bCreateSessionOnDestroy = true;

		SessionCreateInfo* SessionInfo = new SessionCreateInfo();

		SessionInfo->NumPublicConnections = NumPublicConnections;
		SessionInfo->SessionName = NameOfSession;
		SessionInfo->MatchType = MatchType;

		LastSessionCreateInfo = MakeShareable(SessionInfo);

		DestroySession(NameOfSession);
		return;
	}

	ADD_DELEGATE_HANDLE(CreateSessionComplete);

	LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
	LastSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
	LastSessionSettings->NumPublicConnections = NumPublicConnections;
	LastSessionSettings->bAllowJoinInProgress = true;
	LastSessionSettings->bAllowJoinViaPresence = true;
	LastSessionSettings->bShouldAdvertise = true;
	LastSessionSettings->bUsesPresence = true;
	LastSessionSettings->bUseLobbiesIfAvailable = true;
	LastSessionSettings->BuildUniqueId = 1;
	LastSessionSettings->Set(FName("MatchType"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();

	if (!SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), Name, *LastSessionSettings))
	{
		MultiplayerOnCreateSessionComplete.Broadcast(false);
	}
}

void UMultiplayerSessionsSubsystem::FindSessions(int32 MaxSearchResults)
{
	if (!SessionInterface.IsValid())
	{
		MultiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	ADD_DELEGATE_HANDLE(FindSessionsComplete);

	LastSessionSearch = MakeShareable(new FOnlineSessionSearch());

	LastSessionSearch->MaxSearchResults = MaxSearchResults;
	LastSessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
	LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();

	if (!SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef()))
	{
		MultiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
	}
}

void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{
	if (!SessionInterface.IsValid())
	{
		MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}

	ADD_DELEGATE_HANDLE(JoinSessionComplete);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();

	if (!SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), FName(SessionName), SessionResult))
	{
		MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}
}

void UMultiplayerSessionsSubsystem::DestroySession(FString NameOfSession)
{
	if (!SessionInterface.IsValid())
	{
		MultiplayerOnDestroySessionComplete.Broadcast(false);

		if (bCreateSessionOnDestroy)
		{
			bCreateSessionOnDestroy = false;
			MultiplayerOnCreateSessionComplete.Broadcast(false);
		}
		return;
	}

	ADD_DELEGATE_HANDLE(DestroySessionComplete);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.0f,
			FColor::Yellow,
			FString::Printf(TEXT("Destroying session %s..."), *NameOfSession)
		);
	}

	if (!SessionInterface->DestroySession(FName(NameOfSession)))
	{
		MultiplayerOnDestroySessionComplete.Broadcast(false);

		if (bCreateSessionOnDestroy)
		{
			bCreateSessionOnDestroy = false;
			MultiplayerOnCreateSessionComplete.Broadcast(false);
		}
	}
}

void UMultiplayerSessionsSubsystem::StartSession()
{
	if (!SessionInterface.IsValid())
	{
		MultiplayerOnStartSessionComplete.Broadcast(false);
		return;
	}

	ADD_DELEGATE_HANDLE(StartSessionComplete);

	if (!SessionInterface->StartSession(FName(SessionName)))
	{
		MultiplayerOnStartSessionComplete.Broadcast(false);
	}
}

bool UMultiplayerSessionsSubsystem::GetResolvedConnectString(FString& Address)
{
	if (SessionInterface)
	{
		SessionInterface->GetResolvedConnectString(FName(SessionName), Address);
		return true;
	}
	return false;
}

void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName NameOfSession, bool bWasSuccessful)
{
	SessionName = NameOfSession.ToString();
	MultiplayerOnCreateSessionComplete.Broadcast(bWasSuccessful);
}

void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (LastSessionSearch.IsValid())
	{
		MultiplayerOnFindSessionsComplete.Broadcast(LastSessionSearch->SearchResults, bWasSuccessful);
	}
}

void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName NameOfSession, EOnJoinSessionCompleteResult::Type Result)
{
	SessionName = NameOfSession.ToString();
	MultiplayerOnJoinSessionComplete.Broadcast(Result);
}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName NameOfSession, bool bWasSuccessful)
{
	MultiplayerOnDestroySessionComplete.Broadcast(bWasSuccessful);

	if (bCreateSessionOnDestroy)
	{
		bCreateSessionOnDestroy = false;

		if (bWasSuccessful)
		{
			CreateSession(
				LastSessionCreateInfo->NumPublicConnections,
				LastSessionCreateInfo->SessionName,
				LastSessionCreateInfo->MatchType);

			LastSessionCreateInfo.Reset();
		}
		else 
		{
			MultiplayerOnCreateSessionComplete.Broadcast(false);
		}
	}
}

void UMultiplayerSessionsSubsystem::OnStartSessionComplete(FName NameOfSession, bool bWasSuccessful)
{
	MultiplayerOnStartSessionComplete.Broadcast(bWasSuccessful);
}
