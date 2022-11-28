// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "SessionEntry.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class SAUNALAUTTANETWORKING_API USessionEntry : public UUserWidget
{
private:
	GENERATED_BODY()


public:
	void Setup(const FOnlineSessionSearchResult& Result);
	
	UPROPERTY(meta=(BindWidget))
	class UButton* JoinButton;
private:
	UFUNCTION()
	void JoinSessionButton();

	// Store the session search result
	const FOnlineSessionSearchResult* Current;


protected:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* SessionText;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* PlayersText;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* HostText;

};
