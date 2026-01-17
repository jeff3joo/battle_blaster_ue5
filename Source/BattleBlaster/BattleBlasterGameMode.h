// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "Tank.h"
#include "ScreenMessage.h"

#include "BattleBlasterGameMode.generated.h"


/**
 * 
 */
UCLASS()
class BATTLEBLASTER_API ABattleBlasterGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:

	UPROPERTY(EditAnywhere)
	TSubclassOf<UScreenMessage> ScreenMessageClass;

	UScreenMessage* ScreenMessageWidget;

	UPROPERTY(EditAnywhere)
	float GameOverTimer = 3.0f;
	
	UPROPERTY(EditAnywhere)
	int32 CountdownDelay = 3;
	int32 CountdownSeconds;

	FTimerHandle CountdownTimerHandle;

	bool IsVictory = false;
	//int32 CurrentLevelIndex = 1;

	ATank* Tank;
	int32 TowerCount;

	void ActorDied(AActor* DeadActor);
	void OnGameOverTimerTimeout();
	void OnCountdownTimerTimeout();
};
