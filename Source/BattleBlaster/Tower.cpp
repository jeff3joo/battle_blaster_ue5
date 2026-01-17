// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower.h"

void ATower::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle FireTimeHandle;
	GetWorldTimerManager().SetTimer(FireTimeHandle, this, &ATower::CheckFireCondition, FireRate, true);
}

bool ATower::isInFireRange()
{
	if (!Tank->IsAlive) return false;
	FVector TankLocation = Tank->GetActorLocation();
	float DistanceToTank = FVector::Dist(GetActorLocation(), TankLocation);
	
	return DistanceToTank <= FireRange;
}

void ATower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (Tank && isInFireRange()) {
		RotateTurret(Tank->GetActorLocation());
	}
}

void ATower::CheckFireCondition()
{
	if (Tank && isInFireRange()) {
		Fire();
	}
}

void ATower::HandleDestruction()
{
	Super::HandleDestruction();
	
	Destroy();
}
