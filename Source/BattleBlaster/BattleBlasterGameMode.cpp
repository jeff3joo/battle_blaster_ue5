// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleBlasterGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Tower.h"
#include "BattleBlasterGameInstance.h"
#include "Engine/World.h"

void ABattleBlasterGameMode::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> Towers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATower::StaticClass(), Towers);
	TowerCount = Towers.Num();

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (PlayerPawn) {
		Tank = Cast<ATank>(PlayerPawn);
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
		
		// Capture spawn info BEFORE destruction
		const FTransform SpawnTransform = DeadTower->GetActorTransform();
		TSubclassOf<AActor> TowerToSpawnClass = DeadTower->GetClass();

		DeadTower->HandleDestruction();

		// Spawn a new tower after 5 seconds at the same transform and of the same class
		FTimerHandle RespawnHandle;
		const float RespawnDelay = 5.0f;
		FTimerDelegate RespawnDelegate = FTimerDelegate::CreateLambda([this, SpawnTransform, TowerToSpawnClass]()
		{
			if (!TowerToSpawnClass) return;

			UWorld* World = GetWorld();
			if (!World) return;

			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			AActor* NewActor = World->SpawnActor<AActor>(TowerToSpawnClass, SpawnTransform, SpawnParams);
			if (NewActor)
			{
				ATower* NewTower = Cast<ATower>(NewActor);
				if (NewTower)
				{
					NewTower->Tank = Tank;
				}
			}
		});

		GetWorldTimerManager().SetTimer(RespawnHandle, RespawnDelegate, RespawnDelay, false);
	}

	if (IsGameOver)
	{
		ScreenMessageWidget->SetVisibility(ESlateVisibility::Visible);
		FString GameOverString = "Defeat";
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

	BattleBlasterGameInstance->RestartGame();
}

