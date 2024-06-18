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
	static constexpr int8 Player1 = 0;
	static constexpr int8 AI = 1;
};

UCLASS()
class CHESS_API AChess_GameMode : public AGameModeBase
{
	GENERATED_BODY()

private:

	AChess_GameMode();

	// Generate the piece name
	FString GetPieceName(AChessPieces* Piece);

public:
	// Generate the FEN (Forsyth-Edwards Notation) string
	FString GenerateString(const FBoard& Board);

	// Array of strings to keep track of the state of the chessboard
	TArray<FString> FEN_Array;

	int32 CurrentPlayer;

	// Array of player interfaces
	TArray<IChess_PlayerInterface*> Players;

	// TSubclassOf is a template class that provides UClass type safety.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AGameField> GameFieldClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AManagePiece> ManagePieceClass;

	// Field size
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 FieldSize;

	// Reference to a GameField object
	UPROPERTY(VisibleAnywhere)
	AGameField* GField;

	// Reference to a ManagePiece object
	UPROPERTY(VisibleAnywhere)
	AManagePiece* ManagePiece;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;

	// Called at the start of the game
	void ChoosePlayerAndStartGame();

	// Get the next player index
	int32 GetNextPlayer(int32 Player);

	// Called at the end of the game turn
	void TurnNextPlayer();

	// Check if is draw
	bool IsDraw(const FBoard& Board);
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