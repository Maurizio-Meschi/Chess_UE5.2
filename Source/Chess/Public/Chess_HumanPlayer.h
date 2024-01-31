// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Chess_PlayerInterface.h"
//#include "Chess_GameInstance.h"
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

	UPROPERTY(VisibleAnywhere)
	bool PieceChoose;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AChessPieces* CurrPiece;

	TArray<FString> Actor{ "BP_b_Rook_C", "BP_b_Knight_C", "BP_b_Bishop_C", "BP_b_Queen_C", "BP_b_Pawn_C", "BP_b_King_C" };
	TArray<FString> TileActorArray{ "BP_TileBlack_C", "BP_TileWhite_C" };

	//UChess_GameInstance* GameInstance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool MyTurn = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void OnTurn() override;
	virtual void OnWin() override;
	virtual void OnLose() override;

	bool FindPiece(FString ClassName);
	bool FindTile(FString ClassName);

	// called on left mouse click (binding)
	UFUNCTION()
	void OnClick();
};
