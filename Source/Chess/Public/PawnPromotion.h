// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "ChessPieces.h"
//#include "Chess_GameMode.h"
#include "UObject/SoftObjectPtr.h"
#include "UMG.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "PawnPromotion.generated.h"

class AChessPieces;
class AChess_GameMode;

UCLASS()
class CHESS_API UPawnPromotion : public UObject
{
    GENERATED_BODY()

public:

    UPawnPromotion() { Class.SetNum(4); }

    void SetpieceToPromote(AChessPieces* Piece);

    void PawnPromotion();
  
    void PawnPromotionHuman();
    
    void PawnPromotionBot();

    void SpawnNewPiece(TSubclassOf<AChessPieces> PieceClass);
    UFUNCTION()
    void ManageQueenButton();
    UFUNCTION()
    void ManageRookButton();
    UFUNCTION()
    void ManageBishopButton();
    UFUNCTION()
    void ManagePawnButton();

    AChessPieces* PieceToPromote;

    UUserWidget* WidgetGraph;

    TArray< TSubclassOf<AChessPieces>> Class;

    AChess_GameMode* GMode;

    bool IsHumanPlayer = false;
};
