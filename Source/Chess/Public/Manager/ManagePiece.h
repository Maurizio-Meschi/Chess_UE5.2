// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../MainActor/ChessPieces.h"
#include "GameFramework/Actor.h"
#include "ManagePiece.generated.h"


class AChess_GameMode;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDisableButtonDelegate);

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

	FTimerHandle TimerHandle;

	UFUNCTION(BlueprintCallable)
	void DeleteTime();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	AChessPieces* PawnToPromote;

	FString Capture;

public:	

	UPROPERTY(Transient)
	TArray<FRewind> ArrayOfPlays;

	UPROPERTY(BlueprintAssignable, Category = "Events Disable")
	FDisableButtonDelegate DisableButtonEvent;

	static bool Castling;

	bool IsGameOver;

	int32 MoveCounter;

	int32 ButtonValue;

	bool Visible;

	bool IsBotPlayed;

	UPROPERTY(Transient)
	TArray<AChessPieces*> CapturedPieces;

	UPROPERTY(Transient)
	TArray<AChessPieces*> PromotePieces;

	TArray<TArray<FMarked>> LegalMoveArray;

	TArray<TArray<FMarked>>& GetAllLegalMoveByPlayer(FBoard& Board, int8 Player);

	TArray<FMarked>& GetLegalMoveByPiece(FBoard& Board, int32 Player, AChessPieces* Piece);

	void ResetLegalMoveArray();

	void MovePiece(const int32 PlayerNumber, AChessPieces* Piece, FVector2D Coord, FVector2D StartPosition);

	void CapturePiece(AChessPieces* PieceToCapture, FVector2D Coord);

	void ResetData();

	UFUNCTION(BlueprintCallable)
	void Replay();

	UFUNCTION(BlueprintCallable, Category = "Replay Category")
	void BackToPlay();

	UFUNCTION(BlueprintCallable)
	AChessPieces* GetPieceToPromote() { return PawnToPromote; }

	UFUNCTION(BlueprintCallable, Category = "Button Category")
	void SpawnNewPiece(AChessPieces* PieceToPromote, FString NewPiece);

	UFUNCTION(BlueprintCallable, Category = "Button Category")
	void SetButtonValue(int32 Value) { ButtonValue = Value; }

	UFUNCTION(BlueprintCallable, Category = "Button Category")
	bool GetVisibleValue() { return Visible; }

	UFUNCTION(BlueprintCallable, Category = "Button Category")
	bool GetIsBotPlayed() { return IsBotPlayed; }

protected:

	void CheckWinAndGoNextPlayer();

	void RewindManager(int32 MoveNumber);

	void CastlingManager(FVector2D StartRookCoord, FVector2d NewRookCoord, FVector2D Coord, AChessPieces* Piece);
};
