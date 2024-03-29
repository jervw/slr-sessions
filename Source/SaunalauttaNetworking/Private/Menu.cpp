// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"

#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "SessionEntry.h"
#include "Components/TextBlock.h"

void UMenu::Setup(int32 NumberOfConnections, FString TypeOfMatch, FString Level)
{
	MaxConnections = NumberOfConnections;
	MatchType = TypeOfMatch;
	LevelPath = FString::Printf(TEXT("%s?listen"), *Level);

	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeData);
			PlayerController->bShowMouseCursor = true;
		}
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		MultiplayerSubsystem = GameInstance->GetSubsystem<UMultiplayerSubsystem>();
	}

	if (MultiplayerSubsystem)
	{
		// Bind the MultiplayerSubsystem callbacks to their respective functions
		MultiplayerSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		MultiplayerSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		MultiplayerSubsystem->MultiplayerStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
		MultiplayerSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessions);
		MultiplayerSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
	}
}

bool UMenu::Initialize()
{
	if (!Super::Initialize()) return false;

	// Bind the buttons to their respective functions
	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &ThisClass::HostServerButton);
	}

	if (RefreshButton)
	{
		RefreshButton->OnClicked.AddDynamic(this, &ThisClass::RefreshServerButton);
	}

	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinServerButton);
	}

	return true;
}

void UMenu::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	RemoveFromViewport();
	bIsFocusable = false;

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			const FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->bShowMouseCursor = false;
		}
	}

	Super::OnLevelRemovedFromWorld(InLevel, InWorld);
}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			World->ServerTravel(FString::Printf(TEXT("%s?listen"), *LevelPath));
		}
	}
	else if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Failed to create session"));
		HostButton->SetIsEnabled(true);
	}
}

void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	if (!MultiplayerSubsystem) return;

	// Clear the list of sessions
	SessionList->ClearChildren();

	for (auto& Result : SessionResults)
	{
		FString SettingsValue;
		Result.Session.SessionSettings.Get(TEXT("MatchType"), SettingsValue);
		if (SettingsValue == MatchType)
		{
			USessionEntry* SessionEntry = CreateWidget<USessionEntry>(this, SessionEntryClass);
			if (SessionEntry)
			{
				// setup the session result in setup
				SessionEntry->Setup(Result);
				SessionList->AddChildToVerticalBox(SessionEntry);
			}
		}
	}
	JoinButton->SetIsEnabled(!bWasSuccessful);
}


void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	if (IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get())
	{
		IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
		if (SessionInterface)
		{
			FString ConnectAddress;
			if (SessionInterface->GetResolvedConnectString(NAME_GameSession, ConnectAddress))
			{
				auto* PlayerController = GetWorld()->GetFirstPlayerController();
				if (PlayerController)
				{
					PlayerController->ClientTravel(ConnectAddress, ETravelType::TRAVEL_Absolute);
				}
			}
		}
	}

	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		RefreshButton->SetIsEnabled(true);
	}
}

void UMenu::OnDestroySession(bool bWasSuccessful)
{
}

void UMenu::OnStartSession(bool bWasSuccessful)
{
}

void UMenu::HostServerButton()
{
	if (!MultiplayerSubsystem) return;
	HostButton->SetIsEnabled(false);

	MultiplayerSubsystem->CreateSession(MaxConnections, MatchType);
}


void UMenu::RefreshServerButton()
{
	if (!MultiplayerSubsystem) return;

	MultiplayerSubsystem->FindSessions(1000);
}

void UMenu::JoinServerButton()
{
	if (!MultiplayerSubsystem) return;

	MultiplayerSubsystem->FindSessions(1000);
}

void UMenu::DebugFunction()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Debug Function"));
}
