// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleBlasterGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UBattleBlasterGameInstance::ChangeLevel(int32 Index)
{
	FString LevelNameString = FString::Printf(TEXT("Level_%d"), CurrentLevelIndex);

	UGameplayStatics::OpenLevel(GetWorld(), *LevelNameString);
}


void UBattleBlasterGameInstance::RestartGame()
{
	ChangeLevel(1);
}