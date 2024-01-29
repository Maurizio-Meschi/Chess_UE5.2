// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Chess_PlayerInterface.h"
#include "GameFramework/GameModeBase.h"
#include "GameField.h"
#include "Chess_GameMode.generated.h"

/**
 * 
 */
UCLASS()
class CHESS_API AChess_GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	// tracks if the game is over
	bool IsGameOver;
	// array of player interfaces
	TArray<IChess_PlayerInterface*> Players;
	int32 CurrentPlayer;
	// tracks the number of moves in order to signal a drawn game
	//int32 MoveCounter;

	// TSubclassOf is a template class that provides UClass type safety.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AGameField> GameFieldClass;

	// field size
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 FieldSize;

	// reference to a GameField object
	UPROPERTY(VisibleAnywhere)
	AGameField* GField;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AChessPieces> B_Rook;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AChessPieces> B_Knight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AChessPieces> B_Bishop;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AChessPieces> B_Queen;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AChessPieces> B_King;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AChessPieces> B_Pawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AChessPieces> W_Rook;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AChessPieces> W_Knight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AChessPieces> W_Bishop;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AChessPieces> W_Queen;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AChessPieces> W_King;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AChessPieces> W_Pawn;

	AChess_GameMode();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// called at the start of the game
	//void ChoosePlayerAndStartGame();

	// set the cell sign and the position 
	//void SetCellSign(const int32 PlayerNumber, const FVector& SpawnPosition);

	// get the next player index
	//int32 GetNextPlayer(int32 Player);

	// called at the end of the game turn
	//void TurnNextPlayer();
	
};
