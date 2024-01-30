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

	// Vector with tile marked
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<ATile*> TileMarked;

	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	// set chess piece position
	void SetGridPosition(const double InX, const double InY);

	// set chess piece color
	void SetColor(EPieceColor color);

	// 
	void ResetTileMarked();

	FVector2D GetGridPosition()
	{
		return PieceGridPosition;
	}

	virtual void LegalMove() {};

	UFUNCTION()
	void PieceDestroy();
};