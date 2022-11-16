// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MultiplayerSubsystem.h"
#include "SessionEntry.h"
#include "Components/Button.h"
#include "Blueprint/UserWidget.h"
#include "Components/ListView.h"
#include "Components/VerticalBox.h"
#include "Menu.generated.h"


UCLASS()
class USessionEntryData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FString SessionName;
	UPROPERTY()
	FString HostName;
	UPROPERTY()
	FString CurrentPlayers;
	UPROPERTY()
	FString MaxPlayers;
};

/**
 *  This class is the base class for the session menu
 */
UCLASS()
class SAUNALAUTTANETWORKING_API UMenu : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void Setup(int32 NumberOfConnections = 10, FString TypeOfMatch = FString(TEXT("Royale")),
	           FString Level = FString(TEXT("/Game/")));

protected:
	virtual bool Initialize() override;
	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) override;

	// Callbacks for the custom delegates on the custom MultiplayerSubsystem.
	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);
	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);
	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);

	// These callbacks do not require to be UFUNCTIONs, because their delegate types are dynamic.
	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
	void OnFindSessionsOld(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USessionEntry> SessionEntryClass;

private:
	UPROPERTY(meta = (BindWidget))
	UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
	UButton* JoinButton;
	
	UPROPERTY(meta = (BindWidget))
	UButton* RefreshButton;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* SessionList;


	UFUNCTION()
	void HostServerButton();

	UFUNCTION()
	void RefreshServerButton();

	UFUNCTION()
	void JoinServerButton();
	


	//The multiplayer subsystem that is used to host and join sessions
	UMultiplayerSubsystem* MultiplayerSubsystem;

	int32 MaxConnections;
	FString MatchType;
	FString LevelPath;
};
