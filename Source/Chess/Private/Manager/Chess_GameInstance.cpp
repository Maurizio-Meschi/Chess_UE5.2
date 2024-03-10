// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/Manager/Chess_GameInstance.h"



void UChess_GameInstance::SetInfo(FString Message)
{
	Info = Message;
}

FString UChess_GameInstance::GetInfo()
{
	return Info;
}

FString UChess_GameInstance::GetTurnMessage()
{
	return CurrentTurnMessage;
}

void UChess_GameInstance::SetTurnMessage(FString Message)
{
	CurrentTurnMessage = Message;
}
