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
#include "Chess_PlayerController.h"
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

void UPawnPromotion::PawnPromotionHuman()
{
    GMode = Cast<AChess_GameMode>(GWorld->GetAuthGameMode());
    if (GMode == nullptr) UE_LOG(LogTemp, Error, TEXT("GMode nullptr nuu!"));
    AChess_PlayerController* ChessPlayerController = GMode->PlayerController;

    AGameField* Field = GMode->GField;
    if (Class.Num() < 4) Class.SetNum(4);
    Class[0] = (IsHumanPlayer ? Field->GameFieldSubClass.ChessQueen[0] : Field->GameFieldSubClass.ChessQueen[1]);
    Class[1] = (IsHumanPlayer ? Field->GameFieldSubClass.ChessRook[0] : Field->GameFieldSubClass.ChessRook[1]);
    Class[2] = (IsHumanPlayer ? Field->GameFieldSubClass.ChessBishop[0] : Field->GameFieldSubClass.ChessBishop[1]);
    Class[3] = (IsHumanPlayer ? Field->GameFieldSubClass.ChessPawn[0] : Field->GameFieldSubClass.ChessPawn[1]);

    if (ChessPlayerController)
        WidgetGraph = ChessPlayerController->GetInvetoryWidget();
    else 
        UE_LOG(LogTemp, Error, TEXT("PlayerControll Nulla"));
    UE_LOG(LogTemp, Error, TEXT("Human"));
    if (WidgetGraph)
    {
        //WidgetGraph->SetVisibility(ESlateVisibility::Visible);
        UE_LOG(LogTemp, Error, TEXT("Prima del viewport"));
        //WidgetGraph->AddToViewport();
        WidgetGraph->AddToViewport();

        UE_LOG(LogTemp, Error, TEXT("Prima del bottone"));
        UButton* QueenButton = Cast<UButton>(WidgetGraph->GetWidgetFromName(TEXT("QueenButton")));
        if (QueenButton && !QueenButton->OnClicked.IsBound())
        {
            QueenButton->OnClicked.AddDynamic(this, &UPawnPromotion::ManageQueenButton);
        }
        UE_LOG(LogTemp, Error, TEXT("Prima del secondo bottone"));
        UButton* RookButton = Cast<UButton>(WidgetGraph->GetWidgetFromName(TEXT("RookButton")));
        if (RookButton && !RookButton->OnClicked.IsBound())
        {
            RookButton->OnClicked.AddDynamic(this, &UPawnPromotion::ManageRookButton);
        }

        UButton* BishopButton = Cast<UButton>(WidgetGraph->GetWidgetFromName(TEXT("BishopButton")));
        if (BishopButton && !BishopButton->OnClicked.IsBound())
        {
            BishopButton->OnClicked.AddDynamic(this, &UPawnPromotion::ManageBishopButton);
        }
        UE_LOG(LogTemp, Error, TEXT("Prima ultimo bottone"));
        UButton* PawnButton = Cast<UButton>(WidgetGraph->GetWidgetFromName(TEXT("PawnButton")));
        if (PawnButton && !PawnButton->OnClicked.IsBound())
        {
            PawnButton->OnClicked.AddDynamic(this, &UPawnPromotion::ManagePawnButton);
        }
    }
    else UE_LOG(LogTemp, Error, TEXT("sono nullo coco"));
}

void UPawnPromotion::PawnPromotionBot()
{
}

void UPawnPromotion::ManageQueenButton()
{
    SpawnNewPiece(Class[0]);
    UE_LOG(LogTemp, Error, TEXT("prima del broadcast!"));
    GMode->MutexForDelegate.Lock();
    OnPromotionCompleted.Broadcast();
    GMode->MutexForDelegate.Unlock();
}

void UPawnPromotion::ManageRookButton()
{
    SpawnNewPiece(Class[1]);
    GMode->MutexForDelegate.Lock();
    OnPromotionCompleted.Broadcast();
    GMode->MutexForDelegate.Unlock();
}

void UPawnPromotion::ManageBishopButton()
{
    SpawnNewPiece(Class[2]);
    GMode->MutexForDelegate.Lock();
    OnPromotionCompleted.Broadcast();
    GMode->MutexForDelegate.Unlock();
}

void UPawnPromotion::ManagePawnButton()
{
    SpawnNewPiece(Class[3]);
    GMode->MutexForDelegate.Lock();
    OnPromotionCompleted.Broadcast();
    GMode->MutexForDelegate.Unlock();
}

void UPawnPromotion::SpawnNewPiece(TSubclassOf<AChessPieces> PieceClass)
{
    //auto GMode = Cast<AChess_GameMode>(GWorld->GetAuthGameMode());
    AGameField* Field = GMode->GField;

    TMap<FVector2D, ATile*> TileMap = Field->GetTileMap();
    //TMap<FVector2D, AChessPieces*> PiecesMap = Field->GetPiecesMap();

    auto Position = PieceToPromote->GetGridPosition();
    PieceToPromote->SetActorHiddenInGame(true);
    PieceToPromote->SetActorEnableCollision(false);
    GMode->CriticalSection.Lock();
    Field->PiecesMapRemove(Position);
    if (PieceToPromote->Color == EPieceColor::BLACK)
        Field->BotPiecesRemove(PieceToPromote);
    else
        Field->HumanPlayerPiecesRemove(PieceToPromote);
    GMode->CriticalSection.Unlock();
    Field->GenerateChessPieceInXYPosition(Position.X, Position.Y, PieceClass, IsHumanPlayer ? EPieceColor::WHITE : EPieceColor::BLACK);
    //WidgetGraph->SetVisibility(ESlateVisibility::Hidden);
    
    FRewind Obj;
    Obj.PieceToRewind = PieceToPromote;
    Obj.Position = PieceToPromote->GetGridPosition();
    GMode->ArrayOfPlays.Add(Obj);

    WidgetGraph->RemoveFromParent();
          
    //WidgetGraph->RemoveFromParent();
}