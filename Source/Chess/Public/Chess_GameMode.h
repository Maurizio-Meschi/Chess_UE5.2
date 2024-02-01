// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Chess_PlayerInterface.h"
#include "GameFramework/GameModeBase.h"
#include "GameField.h"
#include "Chess_GameInstance.h"
#include "Chess_PlayerController.h"
#include "Chess_GameMode.generated.h"

/**
 * 
 */
UCLASS()
class CHESS_API AChess_GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	// tracks if the game is over
	bool IsGameOver;
	// array of player interfaces
	TArray<IChess_PlayerInterface*> Players;
	int32 CurrentPlayer;
	// tracks the number of moves in order to signal a drawn game
	//int32 MoveCounter;

	// TSubclassOf is a template class that provides UClass type safety.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AGameField> GameFieldClass;

	// field size
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 FieldSize;

	// reference to a GameField object
	UPROPERTY(VisibleAnywhere)
	AGameField* GField;

	AChess_GameMode();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// called at the start of the game
	void ChoosePlayerAndStartGame();

	// set the cell where place the chess piece
	// I need the new position to place the piece and the old position to remove the piece that is moving
	void MovePiece(const int32 PlayerNumber, const FVector& SpawnPosition, AChessPieces* Piece, FVector2D Coord);

	void CapturePiece(AChessPieces* PieceToCapture, FVector2D Coord);

	// get the next player index
	int32 GetNextPlayer(int32 Player);

	// called at the end of the game turn
	void TurnNextPlayer();
	
};