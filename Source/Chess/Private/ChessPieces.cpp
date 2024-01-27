// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessPieces.h"
#include "GameField.h"
#include "Chess_GameMode.h"

// Sets default values
AChessPieces::AChessPieces()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
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

void AChessPieces::PieceDestroy()
{
	Destroy();
}

// ROOK

ARook::ARook() 
{
	Value = 5;
}

bool ARook::LegalMove()
{
	return 1;
}

void ARook::Capture() 
{

}

// Knight

AKnight::AKnight()
{
	Value = 3;
}

bool AKnight ::LegalMove()
{
	return 1;
}

void AKnight::Capture()
{

}

// Bishop

ABishop::ABishop()
{
	Value = 3;
}

bool ABishop::LegalMove()
{
	return 1;
}

void ABishop::Capture()
{

}

// Queen

AQueen::AQueen()
{
	Value = 9;
}

bool AQueen::LegalMove()
{
	return 1;
}

void AQueen::Capture()
{

}

// King

AKing::AKing()
{
	Value = 3;
}

bool AKing::LegalMove()
{
	return 1;
}

void AKing::Capture()
{

}

// APawn

AChessPawn::AChessPawn()
{
	Value = 3;
}

bool AChessPawn::LegalMove()
{
	return 1;
}

void AChessPawn::Capture()
{

}