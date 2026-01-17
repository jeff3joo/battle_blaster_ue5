// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleBlasterGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Tower.h"
#include "BattleBlasterGameInstance.h"

void ABattleBlasterGameMode::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> Towers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATower::StaticClass(), Towers);
	TowerCount = Towers.Num();

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (PlayerPawn) {
		Tank = Cast<ATank>(PlayerPawn);
		if (!Tank)
		{
			UE_LOG(LogTemp, Display, TEXT("GameMode:: Failed to Locate The Tank..."));
		}
	}

	int32 LoopIndex = 0;
	while (LoopIndex < TowerCount)
	{
		AActor* TowerActor = Towers[LoopIndex];
		if (TowerActor)
		{
			ATower* Tower = Cast<ATower>(TowerActor);
			if(Tower) Tower->Tank = Tank;
		}
		LoopIndex++;
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController)
	{
		ScreenMessageWidget = CreateWidget<UScreenMessage>(PlayerController, ScreenMessageClass);
		if (ScreenMessageWidget) 
		{
			ScreenMessageWidget->AddToPlayerScreen();
			ScreenMessageWidget->SetMessage("Get Ready!!");
		}
	}

	CountdownSeconds = CountdownDelay;
	GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &ABattleBlasterGameMode::OnCountdownTimerTimeout, 1.0f, true);
}

void ABattleBlasterGameMode::OnCountdownTimerTimeout()
{
	CountdownSeconds--;

	if (CountdownSeconds > 0)
	{
		ScreenMessageWidget->SetMessage(FString::FromInt(CountdownSeconds));
	}
	else if (CountdownSeconds == 0)
	{
		ScreenMessageWidget->SetMessage("Go !!!");
		Tank->SetPlayerEnabled(true);
	}
	else 
	{
		ScreenMessageWidget->SetVisibility(ESlateVisibility::Hidden);
		GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
	}

}

void ABattleBlasterGameMode::ActorDied(AActor* DeadActor)
{
	bool IsGameOver = false;

	if (DeadActor == Tank)
	{
		IsGameOver = true;
		Tank->HandleDestruction();
	}
	else 
	{
		ATower* DeadTower = Cast<ATower>(DeadActor);
		if (!DeadTower) return;
		
		DeadTower->HandleDestruction();
		TowerCount--;
		if (TowerCount == 0) {
			IsGameOver = true;
			IsVictory = true;
		}
	}

	if (IsGameOver)
	{
		ScreenMessageWidget->SetVisibility(ESlateVisibility::Visible);
		FString GameOverString = IsVictory ? "Victory" : "Defeat";
		ScreenMessageWidget->SetMessage(GameOverString);

		FTimerHandle GameOverTimeHandle;
		GetWorldTimerManager().SetTimer(GameOverTimeHandle, this, &ABattleBlasterGameMode::OnGameOverTimerTimeout, GameOverTimer, false);
	}
}

void ABattleBlasterGameMode::OnGameOverTimerTimeout()
{
	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance) return;

	UBattleBlasterGameInstance* BattleBlasterGameInstance = Cast<UBattleBlasterGameInstance>(GameInstance);
	if (!BattleBlasterGameInstance) return;

	if (IsVictory) {
		BattleBlasterGameInstance->LoadNextLevel();
	} 
	else
	{
		BattleBlasterGameInstance->RestartCurrentLevel();
	}
}

