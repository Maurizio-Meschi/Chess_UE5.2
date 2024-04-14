// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Chess_PlayerInterface.h"
#include "../Manager/ManagePiece.h"
#include "../Manager/Chess_GameInstance.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Chess_HumanPlayer.generated.h"

class UChess_PlayerInterface;

UCLASS()
class CHESS_API AChess_HumanPlayer : public APawn, public IChess_PlayerInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AChess_HumanPlayer();

	UCameraComponent* Camera;

	const TArray<FString> ACTOR{ "BP_w_Rook_C", "BP_w_Knight_C", "BP_w_Bishop_C", "BP_w_Queen_C", "BP_w_Pawn_C", "BP_w_King_C" };
	const TArray<FString> ENEMY_ACTOR{ "BP_b_Rook_C", "BP_b_Knight_C", "BP_b_Bishop_C", "BP_b_Queen_C", "BP_b_Pawn_C", "BP_b_King_C" };
	const TArray<FString> TILE_ACTOR_ARRAY{ "BP_TileBlack_C", "BP_TileWhite_C" };

	UPROPERTY(VisibleAnywhere)
	UChess_GameInstance* GameInstance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool MyTurn = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool PieceChoose;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AChessPieces* CurrPiece;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Starts the human player's turn
	virtual void OnTurn() override;
	// Signals the victory of the human
	virtual void OnWin() override;

	virtual void OnDraw() override;

	// called on left mouse click (binding)
	UFUNCTION()
	void OnClick();

protected:
	void ResetMarkStatus();

	// Manage the click on the tile
	void ManageClickTile(AActor* HitActor, FString ClassName);
	// Handle clicking on a human piece
	void ManageClickPiece(AActor* HitActor);

	// checks if a piece of the human has been selected
	bool FindPiece(FString ClassName);
	// checks if a tile has been selected
	bool FindTile(FString ClassName);
	// checks if a enemy piece has been selected
	bool FindPieceToCapture(FString ClassName);
};
