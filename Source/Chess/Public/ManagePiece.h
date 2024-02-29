// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChessPieces.h"
#include "GameFramework/Actor.h"
#include "ManagePiece.generated.h"


class AChess_GameMode;

USTRUCT()
struct FRewind
{
	GENERATED_BODY()

public:
	FRewind() = default;

	AChessPieces* PieceToRewind;
	FVector2D Position;
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

	bool IsGameOver;

	int32 Count;

	int32 ButtonValue = 0;

	TArray<AChessPieces*> CapturedPieces;

	TArray<AChessPieces*> PromotePieces;

	void MovePiece(const int32 PlayerNumber, const FVector& SpawnPosition, AChessPieces* Piece, FVector2D Coord);

	void CapturePiece(AChessPieces* PieceToCapture, FVector2D Coord);

	void CheckWinAndGoNextPlayer(const int32 PlayerNumber);

	UFUNCTION(BlueprintCallable)
	void Replay();

	UFUNCTION(BlueprintCallable)
	AChessPieces* GetPieceToPromote() { return PawnToPromote; }

	UFUNCTION(BlueprintCallable)
	void HandlePromotionCompleted();

	UFUNCTION(BlueprintCallable, Category = "Button Category")
	void SpawnNewPiece(AChessPieces* PieceToPromote, FString NewPiece);

	UFUNCTION(BlueprintCallable, Category = "Button Category")
	void SetButtonValue(int32 Value) { ButtonValue = Value; }
};
