// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../MainActor/Tile.h"
#include "Chess_PlayerInterface.h"
#include "../Manager/Chess_GameInstance.h"
#include "../Manager/Chess_GameMode.h"
#include "GameFramework/Pawn.h"
#include "Chess_Minimax.generated.h"

UCLASS()
class CHESS_API AChess_Minimax : public APawn, public IChess_PlayerInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChess_Minimax();

	UChess_GameInstance* GameInstance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Look for the AI move
	virtual void OnTurn() override;
	// Signals the victory of the AI
	virtual void OnWin() override;
	// Signals the defeat of the AI
	virtual void OnLose() override;

private:
	// Boolean that tells if the piece to move has been found
	bool IsPieceChoosen = false;

	// Piece and tile where to move it
	AChessPieces* PieceToMove;
	FMarked TileToMove;

	// Calculate the score of the board
	int32 EvaluateGrid(FBoard& Board);
	// MiniMax algorithm
	int32 MiniMax(FBoard& Board, int32 Depth, int32 alpha, int32 beta, bool IsMax);
	// Find the best play for the AI
	FMarked FindBestMove(FBoard& Board);

	// Responsible for moving the piece
	void MovePiece();
};
