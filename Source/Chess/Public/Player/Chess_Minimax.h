// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../MainActor/Tile.h"
#include "Chess_PlayerInterface.h"
#include "../Manager/Chess_GameInstance.h"
#include "../Manager/Chess_GameMode.h"
#include "GameFramework/Pawn.h"
#include "Chess_Minimax.generated.h"

UCLASS()
class CHESS_API AChess_Minimax : public APawn, public IChess_PlayerInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChess_Minimax();

	UChess_GameInstance* GameInstance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void OnTurn() override;
	virtual void OnWin() override;
	virtual void OnLose() override;

	int32 EvaluateGrid(FBoard& Board);
	bool FindTileBetweenP1P2(const FVector2D& P1, const FVector2D& P2, FBoard& Board);
	int32 MiniMax(FBoard& Board, int32 Depth, int32 alpha, int32 beta, bool IsMax);
	FMarked FindBestMove(FBoard& Board);
};
