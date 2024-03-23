// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Tile.h"
#include "GFieldElement/ManageField.h"
#include "GFieldElement/GFieldSubClass.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Containers/Map.h"
#include "GameField.generated.h"

USTRUCT()
struct FBoard
{
	GENERATED_BODY()

public:
	FBoard() = default;

	TMap<FVector2D, ATile*> Field;
	TMap<FVector2D, AChessPieces*> Pieces;
	TArray<AChessPieces*> CapturedPieces;

	AChessPieces* PieceToMove = nullptr;
	
	bool IsMax = true;
};

UCLASS()
class CHESS_API AGameField : public AElementsToManageField
{
	GENERATED_BODY()

	static constexpr int32 SECOND_ROW_FIELD = 2;
	static constexpr int32 PENULTIMATE_ROW_FIELD = 6;
	static constexpr int32 LAST_ROW_FIELD = 8;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float NormalizedCellPadding;

	// size of field
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Size;

	// TSubclassOf template class that provides UClass type safety
	UPROPERTY(EditAnywhere)
	FGFieldTSubClass GameFieldSubClass;

	// tile padding dimension
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CellPadding;

	// tile size
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TileSize;

	int32 PieceIndexValue;

	// Sets default values for this actor's properties
	AGameField();

	UFUNCTION(BlueprintCallable)
	void ResetField();

	// Called when an instance of this class is placed (in editor) or spawned
	virtual void OnConstruction(const FTransform& Transform) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// generate an empty game field
	void GenerateField();

	// Generrate the tile in x,y coordinates
	void GenerateTileInXYPosition(int32 x, int32 y, TSubclassOf<ATile> TileClass);

	// Generrate the chess piece in x,y coordinates
	void GenerateChessPieceInXYPosition(int32 x, int32 y, TSubclassOf<AChessPieces> TileClass, EPieceColor color);

	// return a (x,y) position given a hit (click) on a field tile
	FVector2D GetPosition(const FHitResult& Hit);

	// return a relative position given (x,y) position
	FVector GetRelativeLocationByXYPosition(const int32 InX, const int32 InY) const;

	// return (x,y) position given a relative position
	FVector2D GetXYPositionByRelativeLocation(const FVector& Location) const;
};
