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
	UPROPERTY(VisibleAnywhere)
	FString ChooseAiPlayer = "Easy";

	// Get from the main menu whether to choose random player, minimax or another mode
	UFUNCTION(BlueprintCallable)
	void SetChooseAiPlayer(FString name) { ChooseAiPlayer = name; }
	
	// Set info about the move
	void SetInfo(FString Message);

	// Get info about the move
	UFUNCTION(BlueprintCallable)
	FString GetInfo();

	// Get the current turn message
	UFUNCTION(BlueprintCallable)
	FString GetTurnMessage();

	// Set the turn message
	UFUNCTION(BlueprintCallable)
	void SetTurnMessage(FString Message);

protected:
	// Info about the move
	UPROPERTY(EditAnywhere)
	FString Info;

	// Message to show every turn
	UPROPERTY(EditAnywhere)
	FString CurrentTurnMessage = "Current Player";

};