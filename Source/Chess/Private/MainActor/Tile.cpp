// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/MainActor/Tile.h"

ATile::ATile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));

	// Every actor has a RootComponent that defines the transform in the World
	SetRootComponent(Scene);
	StaticMeshComponent->SetupAttachment(Scene);

	Status = ETileStatus::EMPTY;

	VirtualStatus = EVirtualOccupied::VIRTUAL_EMPTY;
	PlayerOwner = -1;
	TileGridPosition = FVector2D(0, 0);
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();
}

void ATile::SetTileStatus(const int32 TileOwner, const ETileStatus TileStatus)
{
	PlayerOwner = TileOwner;
	Status = TileStatus;
}

void ATile::SetVirtualStatus(const EVirtualOccupied TileStatus)
{
	VirtualStatus = TileStatus;
}

ETileStatus ATile::GetTileStatus()
{
	return Status;
}

EVirtualOccupied ATile::GetVirtaulStatus()
{
	return VirtualStatus;
}

int32 ATile::GetOwner()
{
	return PlayerOwner;
}

void ATile::SetGridPosition(const double InX, const double InY)
{
	TileGridPosition.Set(InX, InY);
}

FVector2D ATile::GetGridPosition()
{
	return TileGridPosition;
}