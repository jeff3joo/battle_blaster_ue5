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

	// Return the tower's canonical spawn transform (where it should spawn by default).
	// By default this returns the transform the tower had in the editor / BeginPlay.
	void GetTowerSpawnLocationAndRotation(FVector& OutLocation, FRotator& OutRotation) const;

	// Set the location the tower should move to after spawn (the last death location).
	void SetRespawnTarget(const FVector& Target);

	// Provide the last death location so the tower knows where to return after chasing.
	void SetLastDeathLocation(const FVector& Location);

private:
	// Cached initial spawn transform (set in BeginPlay)
	UPROPERTY()
	FTransform InitialSpawnTransform;

	// Respawn target (the last place this tower died). When set the tower will move to this location on spawn.
	UPROPERTY()
	FVector RespawnTargetLocation;

	UPROPERTY()
	bool bHasRespawnTarget = false;

	// Stored last death location (same as RespawnTargetLocation but kept separately for clarity).
	UPROPERTY()
	FVector LastDeathLocation;

	UPROPERTY()
	bool bHasLastDeathLocation = false;

	// Whether the tower has arrived at its last death location (used for "return to post" behavior).
	UPROPERTY()
	bool bAtLastDeathLocation = false;

	void GoToDeathLocation(float DeltaTime);
};
