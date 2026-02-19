// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePawn.h"

#include "Tank.h"

#include "Tower.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEBLASTER_API ATower : public ABasePawn
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

private:
	bool isInFireRange();
	bool isPlayerInRange();
	TArray<AActor*> IgnoredActors;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	float FireRange = 500.0f;

	UPROPERTY(EditAnywhere)
	float PlayerRange = 1000.0f;

	UPROPERTY(EditAnywhere)
	float FireRate = 2.0f;

	UPROPERTY(EditAnywhere)
	float Speed = 100.0f;

	// Distance tolerance to consider respawn move complete
	UPROPERTY(EditAnywhere)
	float RespawnAcceptanceRadius = 10.0f;

	ATank* Tank;

	void CheckFireCondition();
	void HandleDestruction();

	void GetTowerSpawnLocationAndRotation(FVector& OutLocation, FRotator& OutRotation) const;

	void SetRespawnTarget(const FVector& Target);

	void SetLastDeathLocation(const FVector& Location);

private:
	UPROPERTY()
	FTransform InitialSpawnTransform;

	UPROPERTY()
	FVector RespawnTargetLocation;

	UPROPERTY()
	bool bHasRespawnTarget = false;

	UPROPERTY()
	FVector LastDeathLocation;

	UPROPERTY()
	bool bHasLastDeathLocation = false;

	UPROPERTY()
	bool bAtLastDeathLocation = false;

	void GoToDeathLocation(float DeltaTime);
};
