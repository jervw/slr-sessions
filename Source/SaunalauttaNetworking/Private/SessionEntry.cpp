// Fill out your copyright notice in the Description page of Project Settings.


#include "SessionEntry.h"

#include "Menu.h"
#include "Components/TextBlock.h"
#include "OnlineSessionSettings.h"


void USessionEntry::Setup(FOnlineSessionSearchResult Result)
{
	Current = &Result;
	
	// Get the session information from the search result
	FText SessionName = FText::FromString("Royale");
	FText HostName = FText::FromString(Current->Session.OwningUserName);

	const int32 CurrentPlayers = Current->Session.SessionSettings.NumPublicConnections - Current->Session.
		NumOpenPublicConnections;

	FText Players = FText::FromString(
		FString::FromInt(CurrentPlayers) + "/" + FString::FromInt(
			Current->Session.SessionSettings.NumPublicConnections));


	// Set the text of the text blocks
	SessionText->Text = SessionName;
	PlayersText->Text = Players;
	HostText->Text = HostName;

	
}

void USessionEntry::JoinSession()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Joining Session"));
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		GEngine->AddOnScreenDebugMessage(-2, 5.f, FColor::Red, TEXT("GameInstance"));
		
		auto* MultiplayerSubsystem = GameInstance->GetSubsystem<UMultiplayerSubsystem>();
		if (MultiplayerSubsystem && Current)
		{
			GEngine->AddOnScreenDebugMessage(-3, 5.f, FColor::Red, TEXT("Subsystem"));
			MultiplayerSubsystem->JoinSession(*Current);
		}
	}
}

bool USessionEntry::Initialize()
{
	if (!Super::Initialize()) return false;

	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinSession);
	}

	return true;
}
