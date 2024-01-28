// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChessPieces.generated.h"

class AGameField;

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
	AGameField* GField;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector2D PieceGridPosition;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EPieceColor Color;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// set chess piece position
	void SetGridPosition(const double InX, const double InY);

	// set chess piece color
	void SetColor(EPieceColor color);

	FVector2D GetGridPosition()
	{
		return PieceGridPosition;
	}

	UFUNCTION()
	void PieceDestroy();
};

UCLASS()
class CHESS_API ARook : public AChessPieces
{
	GENERATED_BODY()
public:
	ARook();

	// every piece has a value
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Value;

	// defines the movements of the piece
	void LegalMove();

	// defines what happens when the piece is captured
	void Capture();
};

UCLASS()
class CHESS_API AKnight : public AChessPieces
{
	GENERATED_BODY()

public:
	AKnight();

	// every piece has a value
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Value;

	// defines the movements of the piece
	void LegalMove();

	// defines what happens when the piece is captured
	void Capture();
};

UCLASS()
class CHESS_API ABishop : public AChessPieces
{
	GENERATED_BODY()

public:
	ABishop();

	// every piece has a value
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Value;

	// defines the movements of the piece
	void LegalMove();

	// defines what happens when the piece is captured
	void Capture();
};

UCLASS()
class CHESS_API AQueen : public AChessPieces
{
	GENERATED_BODY()

public:
	AQueen();

	// every piece has a value
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Value;

	// defines the movements of the piece
	void LegalMove();

	// defines what happens when the piece is captured
	void Capture();
};

UCLASS()
class CHESS_API AKing : public AChessPieces
{
	GENERATED_BODY()

public:
	AKing();

	// defines the movements of the piece
	void LegalMove();

	// defines what happens when the piece is captured
	void Capture();
};

UCLASS()
class CHESS_API AChessPawn : public AChessPieces
{
	GENERATED_BODY()

public:
	AChessPawn();

	// every piece has a value
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Value;

	// The pawn can make a different move on its first play
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 First_play;

	// defines the movements of the piece
	void LegalMove();

	// defines what happens when the piece is captured
	void Capture();
};