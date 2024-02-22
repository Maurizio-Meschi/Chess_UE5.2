// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreUObject.h"
#include "UObject/SoftObjectPtr.h"
#include "UMG.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Delegates/DelegateCombinations.h" 
#include "PawnPromotion.generated.h"

class AChessPieces;
class AChess_GameMode;
class AManagePiece;

UCLASS()
class CHESS_API UPawnPromotion : public UObject
{
    GENERATED_BODY()

public:

    UPawnPromotion() { }

    void SetpieceToPromote(AChessPieces* Piece);

    void PawnPromotion();

    void SetManager(AManagePiece* ManagerPiece) { Manager = ManagerPiece; }

    //static UPawnPromotion* GetInstance();

    bool IsHumanPlayer = false;

    static void DestroyInstance();

    void SpawnNewPiece(TSubclassOf<AChessPieces> PieceClass);
    UFUNCTION(BlueprintCallable, Category = "Button Category")
    void ManageQueenButton();
    UFUNCTION(BlueprintCallable, Category = "Button Category")
    void ManageRookButton();
    UFUNCTION(BlueprintCallable, Category = "Button Category")
    void ManageBishopButton();
    UFUNCTION(BlueprintCallable, Category = "Button Category")
    void ManagePawnButton();
    UFUNCTION(BlueprintCallable, Category = "Button Category")
    static UPawnPromotion* GetInstance();

private:
    static UPawnPromotion* Instance;

    AManagePiece* Manager;

    void PawnPromotionHuman();
    
    void PawnPromotionBot();

    AChessPieces* PieceToPromote;

    UUserWidget* WidgetGraph;

    TArray<TSubclassOf<AChessPieces>> Class;
};
