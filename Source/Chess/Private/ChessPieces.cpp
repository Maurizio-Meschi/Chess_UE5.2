// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessPieces.h"
#include "GameField.h"
#include "Tile.h"
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
	AChess_GameMode* GameMOde = Cast<AChess_GameMode>(GWorld->GetAuthGameMode());
	//GameMode->GField->OnResetEvent.AddDynamic(this, &ABaseSign::SelfDestroy);
}

bool AChessPieces::CheckCoord(int32 x, int32 y)
{
	if (x < 0 || x > 7 || y < 0 || y > 7) return false;
	return true;
}
// Called every frame
/*
void AChessPieces::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
*/

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