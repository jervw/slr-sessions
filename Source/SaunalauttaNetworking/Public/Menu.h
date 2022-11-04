// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MultiplayerSubsystem.h"
#include "Components/Button.h"
#include "Blueprint/UserWidget.h"
#include "Menu.generated.h"


/**
 *  This class is the base class for the session menu
 */
UCLASS()
class SAUNALAUTTANETWORKING_API UMenu : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void Setup(int32 NumberOfConnections = 10, FString TypeOfMatch = FString(TEXT("Royale")), FString Level = FString(TEXT("/Game/")));

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
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);

private:
	UPROPERTY(meta = (BindWidget))
	UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
	UButton* JoinButton;

	UFUNCTION()
	void HostServerButton();

	UFUNCTION()
	void JoinServerButton();

	//The multiplayer subsystem that is used to host and join sessions
	UMultiplayerSubsystem* MultiplayerSubsystem;

	int32 MaxConnections;
	FString MatchType;
	FString LevelPath;
};
