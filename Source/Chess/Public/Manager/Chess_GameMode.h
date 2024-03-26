// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Player/Chess_PlayerInterface.h"
#include "GameFramework/GameModeBase.h"
#include "../MainActor/GameField.h"
#include "Chess_GameInstance.h"
#include "../Player/Chess_PlayerController.h"
#include "Chess_GameMode.generated.h"

struct Player
{
	static constexpr int8 HUMAN = 0;
	static constexpr int8 AI = 1;
};

UCLASS()
class CHESS_API AChess_GameMode : public AGameModeBase
{
	GENERATED_BODY()

private:

	// tracks the number of moves in order to signal a drawn game
	int32 MoveCounter;

	AChess_GameMode();

public:

	int32 CurrentPlayer;

	// Array of player interfaces
	TArray<IChess_PlayerInterface*> Players;

	// TSubclassOf is a template class that provides UClass type safety.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AGameField> GameFieldClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AManagePiece> ManagerClass;

	// Field size
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 FieldSize;

	// Reference to a GameField object
	UPROPERTY(VisibleAnywhere)
	AGameField* GField;

	// Reference to a ManagePiece object
	UPROPERTY(VisibleAnywhere)
	AManagePiece* Manager;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;

	// Called at the start of the game
	void ChoosePlayerAndStartGame();

	// Get the next player index
	int32 GetNextPlayer(int32 Player);

	// Called at the end of the game turn
	void TurnNextPlayer();
};


class FGameRef
{
private:
	static AChess_GameMode* CachedGameMode;

public:
	static void ResetCachedGameMode() { CachedGameMode = nullptr; }

	// Get game mode reference
	static AChess_GameMode* GetGameMode(UObject* WorldContextObject);

	// Get all game reference
	static bool GetGameRef(UObject* WorldContextObject, AChess_GameMode*& GMode, AGameField*& Field, AManagePiece*& PieceManager, FString Source);
	// Get game field reference
	static bool GetGameField(UObject* WorldContextObject, AGameField*& Field, FString Source);
	// Get piece manager reference
	static bool GetManagePiece(UObject* WorldContextObject, AManagePiece*& ManagePiece, FString Source);
};