// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChessPieces.h"
#include "PawnPromotion.h"
#include "GameFramework/Actor.h"
#include "ManagePiece.generated.h"

//DECLARE_DELEGATE(FFunctionDelegate);

class AChess_GameMode;

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

	//TODO:Provare questo approccio
	//TSharedPtr<UPawnPromotion> PromotionInstance;

	//UPROPERTY(EditDefaultsOnly)
	UPawnPromotion* PromotionInstance;
	//TWeakObjectPtr<UPawnPromotion> PromotionInstance;

	//FCriticalSection CriticalSec;

	bool IsGameOver;
public:	

	void MovePiece(const int32 PlayerNumber, const FVector& SpawnPosition, AChessPieces* Piece, FVector2D Coord);

	void CapturePiece(AChessPieces* PieceToCapture, FVector2D Coord);

	void CheckWinAndGoNextPlayer(const int32 PlayerNumber);

	UFUNCTION()
	void HandlePromotionCompleted();

};
