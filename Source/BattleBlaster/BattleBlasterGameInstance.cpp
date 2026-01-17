// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleBlasterGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UBattleBlasterGameInstance::ChangeLevel(int32 Index)
{
	if (Index <= 0 || Index > LastLevelIndex) return;
	CurrentLevelIndex = Index;
	FString LevelNameString = FString::Printf(TEXT("Level_%d"), CurrentLevelIndex);

	UGameplayStatics::OpenLevel(GetWorld(), *LevelNameString);
}

void UBattleBlasterGameInstance::LoadNextLevel()
{
	if (CurrentLevelIndex == LastLevelIndex)
	{
		RestartGame();
		return;
	}
	ChangeLevel(CurrentLevelIndex + 1);
}

void UBattleBlasterGameInstance::RestartCurrentLevel()
{
	ChangeLevel(CurrentLevelIndex);
}

void UBattleBlasterGameInstance::RestartGame()
{
	ChangeLevel(1);
}