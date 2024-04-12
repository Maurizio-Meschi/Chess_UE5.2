// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Chess_PlayerInterface.h"
#include "../Manager/Chess_GameInstance.h"
#include "../Manager/Chess_GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Chess_RandomPlayer.generated.h"

UCLASS()
class CHESS_API AChess_RandomPlayer : public APawn, public IChess_PlayerInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AChess_RandomPlayer();

	UChess_GameInstance* GameInstance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Look for the AI move
	virtual void OnTurn() override;
	// Signals the victory of the AI
	virtual void OnWin() override;
	// Signals the defeat of the AI
	virtual void OnLose() override;

	virtual void OnDraw() override;

};
