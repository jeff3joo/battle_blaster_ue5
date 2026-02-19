// Fill out your copyright notice in the Description page of Project Settings.

#include "Health.h"
#include "BattleBlasterGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "ScreenMessage.h"

UHealth::UHealth()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHealth::BeginPlay()
{
	Super::BeginPlay();
	
	Health = MaxHealth;
	if (AActor* Owner = GetOwner())
	{
		Owner->OnTakeAnyDamage.AddDynamic(this, &UHealth::OnDamageTaken);
	}

	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(GetWorld());
	if (!GameMode) return;
	BattleBlasterGameMode = Cast<ABattleBlasterGameMode>(GameMode);
}


void UHealth::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UHealth::OnDamageTaken(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f) return;

	Health -= Damage;

	if (!BattleBlasterGameMode) return;
	ATank* DamagedTank = Cast<ATank>(DamagedActor);
	if ( DamagedTank && BattleBlasterGameMode->GamePlayHUDWidget)
	{
		BattleBlasterGameMode->GamePlayHUDWidget->SetHealthBarPercent(Health / MaxHealth);
	}
	if (Health <= 0.0f) BattleBlasterGameMode->ActorDied(DamagedActor);
}

void UHealth::IncreaseHealth()
{
	if (Health >= 100.0f) return;
	Health += 25.0f;
	if (BattleBlasterGameMode->GamePlayHUDWidget)
	{
		BattleBlasterGameMode->GamePlayHUDWidget->SetHealthBarPercent(Health / MaxHealth);
	}
}
