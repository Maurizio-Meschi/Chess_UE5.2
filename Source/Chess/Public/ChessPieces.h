// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tile.h"
//#include "Chess_GameMode.h"
#include "GameFramework/Actor.h"
#include "ChessPieces.generated.h"

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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AChess_GameMode> GameModeClass;

	UPROPERTY(VisibleAnywhere)
	AChess_GameMode* GMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector2D PieceGridPosition;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EPieceColor Color;

	bool IsKing;

	// set chess piece position
	void SetGridPosition(const double InX, const double InY);

	// set chess piece color
	void SetColor(EPieceColor color);

	// checks if the coordinates are part of the board
	bool CheckCoord(int32 x, int32 y);

	FVector2D GetGridPosition()
	{
		return PieceGridPosition;
	}

	virtual void LegalMove(int32 PlayerNumber, bool IsHumanPlayer) {};

	void Mark(int32 x, int32 y, int32 PlayerNumber, bool IsHumanPlayer, bool& Marked);

	void CheckMateSituation(int32 x, int32 y, int32 PlayerNumber, bool IsHumanPlayer, bool& Marked);

	void FindTileBetweenP1P2(const FVector2D& P1, const FVector2D& P2, int32 PlayerNumber);

	UFUNCTION()
	void PieceDestroy();
};