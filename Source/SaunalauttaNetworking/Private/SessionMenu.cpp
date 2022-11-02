// Fill out your copyright notice in the Description page of Project Settings.


#include "SessionMenu.h"

#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

void USessionMenu::Setup(int32 MaxConnections, FString MatchType)
{
	BMaxConnections = MaxConnections;
	BMatchType = MatchType;

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

bool USessionMenu::Initialize()
{
	if (!Super::Initialize()) return false;

	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &ThisClass::HostServerButton);
	}

	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinServerButton);
	}

	return true;
}

void USessionMenu::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
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

void USessionMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			World->ServerTravel("/Game/ThirdPerson/Maps/Lobby?listen");
		}
	}
	else UE_LOG(LogTemp, Error, TEXT("Failed to create session"));
}

void USessionMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	if (!MultiplayerSubsystem) return;
	
	for (auto& Result : SessionResults)
	{
		FString SettingsValue;
		Result.Session.SessionSettings.Get(TEXT("MatchType"), SettingsValue);
		if (SettingsValue == BMatchType)
		{
			MultiplayerSubsystem->JoinSession(Result);
			return;
		}
	}
}

void USessionMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
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
}

void USessionMenu::OnDestroySession(bool bWasSuccessful)
{
}

void USessionMenu::OnStartSession(bool bWasSuccessful)
{
}

void USessionMenu::HostServerButton()
{
	if (!MultiplayerSubsystem) return;

	MultiplayerSubsystem->CreateSession(BMaxConnections, BMatchType);
}

void USessionMenu::JoinServerButton()
{
	if (!MultiplayerSubsystem) return;

	MultiplayerSubsystem->FindSessions(1000);
	
	UE_LOG(LogTemp, Warning, TEXT("Join Server"));
}
