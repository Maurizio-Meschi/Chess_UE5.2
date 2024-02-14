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
    if (IsHumanPlayer)
        PawnPromotionHuman();
    else
        PawnPromotionBot();
}

void UPawnPromotion::PawnPromotionHuman()
{
    AChess_GameMode* GMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
    AChess_PlayerController* ChessPlayerController = GMode->PlayerController;

    AGameField* Field = GMode->GField;
    Class[0] = (IsHumanPlayer ? Field->GameFieldSubClass.ChessQueen[0] : Field->GameFieldSubClass.ChessQueen[1]);
    Class[1] = (IsHumanPlayer ? Field->GameFieldSubClass.ChessRook[0] : Field->GameFieldSubClass.ChessRook[1]);
    Class[2] = (IsHumanPlayer ? Field->GameFieldSubClass.ChessBishop[0] : Field->GameFieldSubClass.ChessBishop[1]);
    Class[3] = (IsHumanPlayer ? Field->GameFieldSubClass.ChessPawn[0] : Field->GameFieldSubClass.ChessPawn[1]);

    if (ChessPlayerController)
        WidgetGraph = ChessPlayerController->GetInvetoryWidget();
    else 
        UE_LOG(LogTemp, Error, TEXT("PlayerControll Nulla"));

    if (WidgetGraph)
    {
       WidgetGraph->SetVisibility(ESlateVisibility::Visible);
        // Metti in pausa il gioco
        //UGameplayStatics::SetGamePaused(GetWorld(), true);
        // Associa la funzione di callback al clic dei bottoni
        UButton* QueenButton = Cast<UButton>(WidgetGraph->GetWidgetFromName(TEXT("QueenButton")));
        if (QueenButton)
        {  
            QueenButton->OnClicked.AddDynamic(this, &UPawnPromotion::ManageQueenButton);
        }
        
        UButton* RookButton = Cast<UButton>(WidgetGraph->GetWidgetFromName(TEXT("RookButton")));
        if (RookButton)
        {
            RookButton->OnClicked.AddDynamic(this, &UPawnPromotion::ManageRookButton);
        }

        UButton* BishopButton = Cast<UButton>(WidgetGraph->GetWidgetFromName(TEXT("BishopButton")));
        if (BishopButton)
        {
            BishopButton->OnClicked.AddDynamic(this, &UPawnPromotion::ManageBishopButton);
        }

        UButton* PawnButton = Cast<UButton>(WidgetGraph->GetWidgetFromName(TEXT("PawnButton")));
        if (PawnButton)
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
    OnPromotionCompleted.Broadcast();
}

void UPawnPromotion::ManageRookButton()
{
    SpawnNewPiece(Class[1]);
    OnPromotionCompleted.Broadcast();
}

void UPawnPromotion::ManageBishopButton()
{
    SpawnNewPiece(Class[2]);
    OnPromotionCompleted.Broadcast();
}

void UPawnPromotion::ManagePawnButton()
{
    SpawnNewPiece(Class[3]);
    OnPromotionCompleted.Broadcast();
}

void UPawnPromotion::SpawnNewPiece(TSubclassOf<AChessPieces> PieceClass)
{
    auto GMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
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
    Field->GenerateChessPieceInXYPosition(Position.X, Position.Y, PieceClass, IsHumanPlayer ? EPieceColor::WHITE : EPieceColor::BLACK);
    GMode->CriticalSection.Unlock();
    WidgetGraph->SetVisibility(ESlateVisibility::Hidden);
    
    FRewind Obj;
    Obj.PieceToRewind = PieceToPromote;
    Obj.Position = PieceToPromote->GetGridPosition();
    GMode->ArrayOfPlays.Add(Obj);
    UE_LOG(LogTemp, Error, TEXT("Fine della spawnPiece"));
}