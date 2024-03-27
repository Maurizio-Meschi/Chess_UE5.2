// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tile.h"

#include "GameFramework/Actor.h"
#include "ChessPieces.generated.h"

struct FBoard;
class AGameField;
class AChess_GameMode;

UENUM()
enum class EPieceColor : uint8
{
	BLACK     UMETA(DisplayName = "Black"),
	WHITE     UMETA(DisplayName = "White"),
};

UCLASS()
class CHESS_API AChessPieces : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChessPieces();

	// Starting position of the left white rook
	const FVector2D HUMAN_ROOK_POSITION1 = FVector2D(0, 0);
	// Starting position of the right white rook
	const FVector2D HUMAN_ROOK_POSITION2 = FVector2D(0, 7);
	// Starting position of the left black rook
	const FVector2D AI_ROOK_POSITION1	 = FVector2D(7, 0);
	// Starting position of the right balck rook
	const FVector2D AI_ROOK_POSITION2	 = FVector2D(7, 7);
	// Starting position of the white king
	const FVector2D HUMAN_KING_POSITION  = FVector2D(0, 4);
	// Starting position of the black king
	const FVector2D AI_KING_POSITION	 = FVector2D(7, 4);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	//UPROPERTY(EditDefaultsOnly)
	//TSubclassOf<AChess_GameMode> GameModeClass;

	// x,y position of the piece
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector2D PieceGridPosition;

	// Piece color
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EPieceColor Color;

	// Piece name
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString Name;

	// Index of the piece in legalmove array
	int32 IndexArray;

	// Piece value
	int32 Value;

	// Set chess piece position
	void SetGridPosition(const double InX, const double InY);

	// Set chess piece color
	void SetColor(EPieceColor color);

	// Get piece position
	FVector2D GetGridPosition()
	{
		return PieceGridPosition;
	}

	/*
	* @param: 1.) Reference to the chessboard
	*         2.) Player
	*         3.) CheckFlag: false if you want to allocate the possible tiles in which to move the piece, 
	*                        true if you want to check if the king is in check
	*
	*  @return: false if CheckFlag is false, true if CheckFlag is true and king is in check
	* 
	*  @note: The method calculates the x,y of the tiles in which the piece can move
	*         and through the support methods MarkTile and CheckTest the legalmove array is filled
	*/
	virtual bool LegalMove(FBoard& Board, int32 PlayerNumber, bool CheckFlag) { return true; }

protected:

	// Checks if the coordinates are part of the board
	bool CheckCoord(int32 x, int32 y);

	// Reset the status of the tiles
	void ResetTileStatus(ATile* CurrTile, ATile* NewTile, int32 PlayerNumber, bool IsTileEmpty);

	// Method for allocating legal moves in the array
	void MarkTile(FBoard& Board, int32 x, int32 y, int32 PlayerNumber, bool& Marked);

	// Method that checks whether the king is in check
	bool TestCheck(FBoard& Board, int32 x, int32 y, int32 PlayerNumber, bool& Marked);

	// 
	void Castling(FBoard& Board, FVector2D TilePosition, FVector2D RookPosition, int32 PlayerNumber, bool& Marked);
};