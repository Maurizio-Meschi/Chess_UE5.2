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

UChess_GameInstance* FGameInstanceRef::CachedGameInstance = nullptr;

UChess_GameInstance* FGameInstanceRef::GetGameInstance(UObject* WorldContextObject)
{
	if (!CachedGameInstance)
	{
		UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
		CachedGameInstance = World->GetGameInstance<UChess_GameInstance>();
	}
	return CachedGameInstance;
}
