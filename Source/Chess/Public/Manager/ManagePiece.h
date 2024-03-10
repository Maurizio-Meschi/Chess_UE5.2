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

	AChessPieces* PawnToPromote;

	FString Capture;

public:	

	UPROPERTY(Transient)
	TArray<FRewind> ArrayOfPlays;

	UPROPERTY(BlueprintAssignable, Category = "Events Disable")
	FDisableButtonDelegate DisableButtonEvent;

	static bool Castling;

	bool IsGameOver;

	int32 Count;

	int32 ButtonValue = 0;

	bool Visible = true;

	bool IsBotPlayed = true;

	UPROPERTY(Transient)
	TArray<AChessPieces*> CapturedPieces;

	UPROPERTY(Transient)
	TArray<AChessPieces*> PromotePieces;

	TArray<TArray<FMarked>> TileMarkedForPiece;

	void MovePiece(const int32 PlayerNumber, const FVector& SpawnPosition, AChessPieces* Piece, FVector2D Coord, FVector2D StartPosition);

	void CapturePiece(AChessPieces* PieceToCapture, FVector2D Coord);

	UFUNCTION(BlueprintCallable)
	void Replay();

	UFUNCTION(BlueprintCallable, Category = "Replay Category")
	void BackToPlay();

	UFUNCTION(BlueprintCallable)
	AChessPieces* GetPieceToPromote() { return PawnToPromote; }

	UFUNCTION(BlueprintCallable)
	void HandlePromotionCompleted();

	UFUNCTION(BlueprintCallable, Category = "Button Category")
	void SpawnNewPiece(AChessPieces* PieceToPromote, FString NewPiece);

	UFUNCTION(BlueprintCallable, Category = "Button Category")
	void SetButtonValue(int32 Value) { ButtonValue = Value; }

	UFUNCTION(BlueprintCallable, Category = "Button Category")
	bool GetVisibleValue() { return Visible; }

	UFUNCTION(BlueprintCallable, Category = "Button Category")
	bool GetIsBotPlayed() { return Visible; }

protected:

	void CheckWinAndGoNextPlayer(const int32 PlayerNumber);

	void RewindManager(int32 MoveNumber);
};
