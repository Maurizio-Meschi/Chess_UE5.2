// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_GameInstance.h"

#include "Chess_GameInstance.h"

void UChess_GameInstance::IncrementNumPlayed()
{
	NumPlayed += 1;
}

int32 UChess_GameInstance::GetNumPlayed()
{
	return NumPlayed;
}

FString UChess_GameInstance::GetTurnMessage()
{
	return CurrentTurnMessage;
}

void UChess_GameInstance::SetTurnMessage(FString Message)
{
	CurrentTurnMessage = Message;
}