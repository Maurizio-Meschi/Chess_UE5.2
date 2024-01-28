// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessPieces.h"
#include "GameField.h"
#include "Chess_GameMode.h"

// Sets default values
AChessPieces::AChessPieces()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	PieceGridPosition = FVector2D(0, 0);

	Color = EPieceColor::BLACK;
}

// Called when the game starts or when spawned

void AChessPieces::BeginPlay()
{
	Super::BeginPlay();
	AChess_GameMode* GameMOde = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
	//GameMode->GField->OnResetEvent.AddDynamic(this, &ABaseSign::SelfDestroy);
}


// Called every frame

void AChessPieces::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AChessPieces::SetGridPosition(const double InX, const double InY)
{
	PieceGridPosition.Set(InX, InY);
}

void AChessPieces::SetColor(EPieceColor color)
{
	Color = color;
}

void AChessPieces::PieceDestroy()
{
	Destroy();
}

// ROOK

ARook::ARook() 
{
	Value = 5;
}

void ARook::LegalMove()
{
	
}

void ARook::Capture() 
{

}

// Knight

AKnight::AKnight()
{
	Value = 3;
}

void AKnight ::LegalMove()
{
	
}

void AKnight::Capture()
{

}

// Bishop

ABishop::ABishop()
{
	Value = 3;
}

void ABishop::LegalMove()
{
	
}

void ABishop::Capture()
{

}

// Queen

AQueen::AQueen()
{
	Value = 9;
}

void AQueen::LegalMove()
{

}

void AQueen::Capture()
{

}

// King

AKing::AKing() {}

void AKing::LegalMove()
{
	
}

void AKing::Capture()
{

}

// APawn

AChessPawn::AChessPawn()
{
	Value = 1;
	First_play = 1;
}

void AChessPawn::LegalMove()
{
	// get the coordinates of the pawn
	FVector2D ChessPawnXYposition = PieceGridPosition;
	int32 x = ChessPawnXYposition[0];
	int32 y = ChessPawnXYposition[1];
	int32 row = 0;
	int32 XYPositionInArray = (x + row) * 8 + y;

	//TODO: avere l'owner della pedina
	int32 tmp_owner = 0;
	// get the tile in the game field 
	TArray<ATile*> TileArray = GField->GetTileArray();

		// check if the next vertical tile is empty and if true, mark the tile
		row = 1;
		XYPositionInArray = (x + row) * 8 + y;
		if (TileArray[XYPositionInArray]->GetTileStatus() == ETileStatus::EMPTY)
		{
			TileArray[XYPositionInArray]->SetTileStatus(tmp_owner, ETileStatus::MARKED);
		}
		// check if the next second vertical tile is empty and if true, mark the tile
		row = 2;
		XYPositionInArray = (x + row) * 8 + y;
		if (TileArray[XYPositionInArray]->GetTileStatus() == ETileStatus::EMPTY && First_play)
		{
			TileArray[XYPositionInArray]->SetTileStatus(tmp_owner, ETileStatus::MARKED);
		}
}

void AChessPawn::Capture()
{

}