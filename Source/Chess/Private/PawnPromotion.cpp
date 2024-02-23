// Fill out your copyright notice in the Description page of Project Settings.


#include "PawnPromotion.h"
#include "Chess_GameMode.h"
#include "Pieces/Bishop.h"
#include "Pieces/ChessPawn.h"
#include "Pieces/King.h"
#include "Pieces/Knight.h"
#include "Pieces/Queen.h"
#include "Pieces/Rook.h"
#include "ChessPieces.h"
#include "ManagePiece.h"
#include "Chess_PlayerController.h"
#include "HAL/PlatformProcess.h"
#include "Containers/UnrealString.h"
#include "UObject/SoftObjectPtr.h"
#include "UMG.h"
#include "Engine/World.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"

void UPawnPromotion::SetpieceToPromote(AChessPieces* Piece)
{
    PieceToPromote = Piece;
}

void UPawnPromotion::PawnPromotion()
{
    UE_LOG(LogTemp, Error, TEXT("Pawn promotion!"));
    if (IsHumanPlayer)
        PawnPromotionHuman();
    else
        PawnPromotionBot();
}

UPawnPromotion* UPawnPromotion::Instance = nullptr;

UPawnPromotion* UPawnPromotion::GetInstance()
{
    if (!Instance)
    {
        // Create the singleton instance if it doesn't exist
        Instance = NewObject<UPawnPromotion>();
    }
    return Instance;
}

void UPawnPromotion::DestroyInstance()
{
    if (Instance)
    {
        Instance->ConditionalBeginDestroy();
        //Instance = nullptr;
    }

}

void UPawnPromotion::PawnPromotionHuman()
{
    auto GMode = FGameModeRef::GetGameMode(this);
    if (!GMode)
    {
        UE_LOG(LogTemp, Error, TEXT("Game mode null in PawnPromotion"));
        return;
    }

    AGameField* Field = GMode->GField;
    if (!Field)
    {
        UE_LOG(LogTemp, Error, TEXT("Field null in PawnPromotion"));
        return;
    }

    AChess_PlayerController* ChessPlayerController = GMode->PlayerController;
    if (!ChessPlayerController)
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerController null in PawnPromotion"));
        return;
    }

    WidgetGraph = ChessPlayerController->GetInvetoryWidget();

    UE_LOG(LogTemp, Error, TEXT("Array"));
    if (Field->GameFieldSubClass.ChessQueen.Num() >= 1)
        Class.Add(Field->GameFieldSubClass.ChessQueen[0]);
    if (Field->GameFieldSubClass.ChessRook.Num() >= 1)
        Class.Add(Field->GameFieldSubClass.ChessRook[0]);
    if (Field->GameFieldSubClass.ChessBishop.Num() >= 1)
        Class.Add(Field->GameFieldSubClass.ChessBishop[0]);
    if (Field->GameFieldSubClass.ChessPawn.Num() >= 1)
        Class.Add(Field->GameFieldSubClass.ChessPawn[0]);

    UE_LOG(LogTemp, Error, TEXT("Human"));
    if (WidgetGraph)
    {
        UE_LOG(LogTemp, Error, TEXT("Prima del viewport"));
        if (!WidgetGraph->IsInViewport())
            WidgetGraph->AddToViewport();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Widget null in PawnPromotion"));
        return;
    }
}

void UPawnPromotion::PawnPromotionBot()
{
    auto GMode = FGameModeRef::GetGameMode(this);
    if (!GMode)
    {
        UE_LOG(LogTemp, Error, TEXT("Game mode null in PawnPromotion"));
        return;
    }

    AGameField* Field = GMode->GField;
    if (!Field)
    {
        UE_LOG(LogTemp, Error, TEXT("Field null in PawnPromotion"));
        return;
    }

    if (Field->GameFieldSubClass.ChessQueen.Num() == 2)
        Class.Add(Field->GameFieldSubClass.ChessQueen[1]);
    if (Field->GameFieldSubClass.ChessRook.Num() == 2)
        Class.Add(Field->GameFieldSubClass.ChessRook[1]);
    if (Field->GameFieldSubClass.ChessBishop.Num() == 2)
        Class.Add(Field->GameFieldSubClass.ChessBishop[1]);
    if (Field->GameFieldSubClass.ChessPawn.Num() == 2)
        Class.Add(Field->GameFieldSubClass.ChessPawn[1]);

    if (Class.Num() > 0)
    {
        int32 RIndex = FMath::Rand() % Class.Num();
        if (Class.Num() > RIndex)
            SpawnNewPiece(Class[RIndex]);
    }
}

void UPawnPromotion::ManageQueenButton()
{
    if (Class.Num() > 0)
        SpawnNewPiece(Class[0]);
}

void UPawnPromotion::ManageRookButton()
{
    if (Class.Num() > 1)
        SpawnNewPiece(Class[1]);
}

void UPawnPromotion::ManageBishopButton()
{
    if (Class.Num() > 2)
        SpawnNewPiece(Class[2]);
}

void UPawnPromotion::ManagePawnButton()
{
    if (Class.Num() > 3)
        SpawnNewPiece(Class[3]);
}

void UPawnPromotion::SpawnNewPiece(TSubclassOf<AChessPieces> PieceClass)
{
    auto GMode = FGameModeRef::GetGameMode(this);
    if (!GMode)
    {
        UE_LOG(LogTemp, Error, TEXT("Game mode null in PawnPromotion"));
        return;
    }

    AGameField* Field = GMode->GField;
    if (!Field)
    {
        UE_LOG(LogTemp, Error, TEXT("Field null in PawnPromotion"));
        return;
    }

    TMap<FVector2D, ATile*> TileMap = Field->GetTileMap();

    auto Position = PieceToPromote->GetGridPosition();
    PieceToPromote->SetActorHiddenInGame(true);
    PieceToPromote->SetActorEnableCollision(false);

    
    Field->PiecesMapRemove(Position);

    if (PieceToPromote->Color == EPieceColor::BLACK)
        Field->BotPiecesRemove(PieceToPromote);
    else
        Field->HumanPlayerPiecesRemove(PieceToPromote);
    

    Field->GenerateChessPieceInXYPosition(Position.X, Position.Y, PieceClass, IsHumanPlayer ? EPieceColor::WHITE : EPieceColor::BLACK);
    
    FRewind Obj;
    Obj.PieceToRewind = PieceToPromote;
    Obj.Position = PieceToPromote->GetGridPosition();
    GMode->ArrayOfPlays.Add(Obj);
    if (WidgetGraph && WidgetGraph->IsInViewport())
    {
        WidgetGraph->RemoveFromParent();
        WidgetGraph = nullptr;
    }
    Class.Empty();
    Manager->HandlePromotionCompleted();
}