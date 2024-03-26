// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../MainActor/ChessPieces.h"
#include "GameFramework/Actor.h"
#include "ManagePiece.generated.h"


class AChess_GameMode;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDisableButtonDelegate);

// Structure to manage the replay
USTRUCT()
struct FRewind
{
	GENERATED_BODY()

public:
	FRewind() = default;

	AChessPieces* PieceToRewind;
	FVector2D Position;
	bool Capture;
	bool Castling = false;
};

// Structure to manage the tiles in which to move the pieces
USTRUCT()
struct FMarked
{
	GENERATED_BODY()

public:
	FMarked() = default;

	ATile* Tile;
	bool Capture;

	bool operator==(const FMarked& other) const {
		return Tile == other.Tile;
	}
};


UCLASS()
class CHESS_API AManagePiece : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AManagePiece();

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	// Timer Handle for the AI player
	FTimerHandle TimerHandle;

	bool IsGameOver;

	// if this is true, castling must be managed
	static bool Castling;

	// Array to track movements
	UPROPERTY(Transient)
	TArray<FRewind> ArrayOfPlays;

	// Event to disable replay while AI plays
	UPROPERTY(BlueprintAssignable, Category = "Events Disable")
	FDisableButtonDelegate DisableButtonEvent;

	UPROPERTY(Transient)
	TArray<AChessPieces*> CapturedPieces;

	UPROPERTY(Transient)
	TArray<AChessPieces*> PromotePieces;

	// Each element of the array corresponds to an array containing the legal moves for the jth piece
	TArray<TArray<FMarked>> LegalMoveArray;

	// Get all legal moves of the specified player's pieces
	TArray<TArray<FMarked>>& GetAllLegalMoveByPlayer(FBoard& Board, int8 Player);

	// Get the legal moves of a specific piece
	TArray<FMarked>& GetLegalMoveByPiece(FBoard& Board, int32 Player, AChessPieces* Piece);

	// Resets the legal move array
	void ResetLegalMoveArray();

	bool IsCheckMate();

	// Move the piece to the new location
	void MovePiece(const int32 PlayerNumber, AChessPieces* Piece, FVector2D Coord, FVector2D StartPosition);

	// Capture an enemy piece
	void CapturePiece(AChessPieces* PieceToCapture, FVector2D Coord);

	// Restore all structures of this class
	void ResetData();

	// Delete the timers associated with the TimerHandle
	UFUNCTION(BlueprintCallable)
	void DeleteTime();

	UFUNCTION(BlueprintCallable)
	void Replay();

	UFUNCTION(BlueprintCallable, Category = "Replay Category")
	void BackToPlay();

	// Spawn a new piece when the pawn is promoted
	UFUNCTION(BlueprintCallable, Category = "Button Category")
	void SpawnNewPiece(AChessPieces* PieceToPromote, FString NewPiece);

	// When the human clicks on a button that indicates the move, the value is saved
	UFUNCTION(BlueprintCallable, Category = "Button Category")
	void SetButtonValue(int32 Value) { ButtonValue = Value; }

	/// Methods to return values for use in blueprints
	UFUNCTION(BlueprintCallable)
	AChessPieces* GetPieceToPromote() { return PawnToPromote; }

	UFUNCTION(BlueprintCallable, Category = "Button Category")
	bool GetVisibleValue() { return Visible; }

	UFUNCTION(BlueprintCallable, Category = "Button Category")
	bool GetIsBotPlayed() { return IsBotPlayed; }

protected:
	AChessPieces* PawnToPromote;

	// Indicates the number of the move
	int32 MoveCounter;

	// The button value represents the i-th move
	int32 ButtonValue;

	// String to manage the nomenclature of the play when a capture occurs
	FString Capture;

	// Booleans used to manage button graphics in the editor
	bool Visible;
	bool IsBotPlayed;

	// Check if it's checkmate. if it is not, it moves on to the next player's turn
	void CheckWinAndGoNextPlayer();

	void RewindManager(int32 MoveNumber);

	void CastlingManager(FVector2D StartRookCoord, FVector2d NewRookCoord, FVector2D Coord, AChessPieces* Piece);
};
