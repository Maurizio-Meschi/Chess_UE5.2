// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Chess_PlayerInterface.h"
#include "GameFramework/GameModeBase.h"
#include "GameField.h"
#include "Chess_GameInstance.h"
#include "Chess_PlayerController.h"
#include "Chess_GameMode.generated.h"

UCLASS()
class CHESS_API AChess_GameMode : public AGameModeBase
{
	GENERATED_BODY()

private:

	// tracks if the game is over
	//bool IsGameOver;

	//int32 CurrentPlayer;

	// tracks the number of moves in order to signal a drawn game
	int32 MoveCounter;

	AChess_GameMode();

public:
	FCriticalSection CriticalSection;

	int32 CurrentPlayer;

	// array of player interfaces
	TArray<IChess_PlayerInterface*> Players;

	// TSubclassOf is a template class that provides UClass type safety.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AGameField> GameFieldClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AManagePiece> ManagerClass;

	// field size
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 FieldSize;

	// reference to a GameField object
	UPROPERTY(VisibleAnywhere)
	AGameField* GField;

	UPROPERTY(VisibleAnywhere)
	AManagePiece* Manager;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;

	// called at the start of the game
	void ChoosePlayerAndStartGame();

	// get the next player index
	int32 GetNextPlayer(int32 Player);

	// called at the end of the game turn
	void TurnNextPlayer();
};


class FGameModeRef
{
private:
	static AChess_GameMode* CachedGameMode;

public:
	static void ResetCachedGameMode() { CachedGameMode = nullptr; }
	static AChess_GameMode* GetGameMode(UObject* WorldContextObject);
};