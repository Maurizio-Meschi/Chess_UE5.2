// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Chess_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class CHESS_API UChess_GameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	// score value for human player
	UPROPERTY(EditAnywhere)
	int32 NumPlayed = 0;

	// message to show every turn
	UPROPERTY(EditAnywhere)
	FString CurrentTurnMessage = "Current Player";

	// increment the score for human player
	void IncrementNumPlayed();

	// get the score for human player
	UFUNCTION(BlueprintCallable)
	int32 GetNumPlayed();

	// get the current turn message
	UFUNCTION(BlueprintCallable)
	FString GetTurnMessage();

	// set the turn message
	UFUNCTION(BlueprintCallable)
	void SetTurnMessage(FString Message);
};
