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

void UPawnPromotion::PawnPromotionHuman()
{
    auto GMode = FGameModeRef::GetGameMode(this);
    if (GMode == nullptr) UE_LOG(LogTemp, Error, TEXT("GMode nullptr nuu!"));
    AChess_PlayerController* ChessPlayerController = GMode->PlayerController;

    AGameField* Field = GMode->GField;
    //if (Class.Num() < 4) Class.SetNum(4);
    Class.Add(Field->GameFieldSubClass.ChessQueen[0]);
    Class.Add(Field->GameFieldSubClass.ChessRook[0]);
    Class.Add(Field->GameFieldSubClass.ChessBishop[0]);
    Class.Add(Field->GameFieldSubClass.ChessPawn[0]);

    if (ChessPlayerController)
        WidgetGraph = ChessPlayerController->GetInvetoryWidget();
    else 
        UE_LOG(LogTemp, Error, TEXT("PlayerControll Nulla"));
    UE_LOG(LogTemp, Error, TEXT("Human"));
    if (WidgetGraph)
    {
        UE_LOG(LogTemp, Error, TEXT("Prima del viewport"));
        if (!WidgetGraph->IsInViewport())
            WidgetGraph->AddToViewport();
        if(GMode->GField == nullptr)
            UE_LOG(LogTemp, Error, TEXT("controllo a caso->sara la viewport?"));

        UE_LOG(LogTemp, Error, TEXT("Prima del bottone"));
        TWeakObjectPtr<UButton> QueenButton = Cast<UButton>(WidgetGraph->GetWidgetFromName(TEXT("QueenButton")));
        if (QueenButton.IsValid() && !QueenButton->OnClicked.IsBound())
        {
            QueenButton->OnClicked.AddDynamic(this, &UPawnPromotion::ManageQueenButton);
        }
        UE_LOG(LogTemp, Error, TEXT("Prima del secondo bottone"));
        TWeakObjectPtr<UButton> RookButton = Cast<UButton>(WidgetGraph->GetWidgetFromName(TEXT("RookButton")));
        if (RookButton.IsValid() && !RookButton->OnClicked.IsBound())
        {
            RookButton->OnClicked.AddDynamic(this, &UPawnPromotion::ManageRookButton);
        }

        TWeakObjectPtr<UButton> BishopButton = Cast<UButton>(WidgetGraph->GetWidgetFromName(TEXT("BishopButton")));
        if (BishopButton.IsValid() && !BishopButton->OnClicked.IsBound())
        {
            BishopButton->OnClicked.AddDynamic(this, &UPawnPromotion::ManageBishopButton);
        }
        UE_LOG(LogTemp, Error, TEXT("Prima ultimo bottone"));
        TWeakObjectPtr<UButton> PawnButton = Cast<UButton>(WidgetGraph->GetWidgetFromName(TEXT("PawnButton")));
        if (PawnButton.IsValid() && !PawnButton->OnClicked.IsBound())
        {
            PawnButton->OnClicked.AddDynamic(this, &UPawnPromotion::ManagePawnButton);
        }
        UE_LOG(LogTemp, Error, TEXT("Bottoni done"));
    }
    else UE_LOG(LogTemp, Error, TEXT("sono nullo coco"));
}

void UPawnPromotion::PawnPromotionBot()
{
    auto GMode = FGameModeRef::GetGameMode(this);
    auto Field = GMode->GField;
    Class.Add(Field->GameFieldSubClass.ChessQueen[1]);
    Class.Add(Field->GameFieldSubClass.ChessRook[1]);
    Class.Add(Field->GameFieldSubClass.ChessBishop[1]);
    Class.Add(Field->GameFieldSubClass.ChessPawn[1]);
    Class.Add(Field->GameFieldSubClass.ChessKnight[1]);
    int32 RIndex = FMath::Rand() % Class.Num();
    SpawnNewPiece(Class[RIndex]);
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
    auto GMode = FGameModeRef::GetGameMode(this);
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
    WidgetGraph = nullptr;
    Class.Empty();
}