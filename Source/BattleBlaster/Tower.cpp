// Fill out your copyright notice in the Description page of Project Settings.

#include "Tower.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/BlockingVolume.h"

void ATower::BeginPlay()
{
	Super::BeginPlay();

	// Cache initial spawn transform so GameMode (or others) can request where this tower should spawn by default.
	InitialSpawnTransform = GetActorTransform();

	// Ensure the root component has collision enabled so sweeping actually blocks movement.
	if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(GetRootComponent()))
	{
		if (Prim->GetCollisionEnabled() == ECollisionEnabled::NoCollision)
		{
			Prim->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			Prim->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		}
	}

	FTimerHandle FireTimeHandle;
	GetWorldTimerManager().SetTimer(FireTimeHandle, this, &ATower::CheckFireCondition, FireRate, true);
}

bool ATower::isInFireRange()
{
	if (!Tank || !Tank->IsAlive) return false;
	FVector TankLocation = Tank->GetActorLocation();
	float DistanceToTank = FVector::Dist(GetActorLocation(), TankLocation);

	return DistanceToTank <= FireRange;
}

bool ATower::isPlayerInRange()
{
	if (!Tank || !Tank->IsAlive) return false;
	FVector TankLocation = Tank->GetActorLocation();
	float DistanceToTank = FVector::Dist(GetActorLocation(), TankLocation);

	return DistanceToTank <= PlayerRange;
}

void ATower::SetRespawnTarget(const FVector& Target)
{
	RespawnTargetLocation = Target;
	bHasRespawnTarget = true;
	bAtLastDeathLocation = false;
}

void ATower::SetLastDeathLocation(const FVector& Location)
{
	LastDeathLocation = Location;
	bHasLastDeathLocation = true;
	// Keep last death info; this is used to return to post after chasing.
}

void ATower::GoToDeathLocation(float DeltaTime)
{
	if (bHasRespawnTarget)
	{
		const FVector Current = GetActorLocation();
		const float Dist = FVector::Dist(Current, RespawnTargetLocation);

		// If close enough, arrival complete.
		if (Dist <= RespawnAcceptanceRadius)
		{
			bHasRespawnTarget = false;
			bAtLastDeathLocation = true;
			// Allow normal behavior next tick.
		}
		else
		{
			// Move toward respawn target with sweep so collisions are respected.
			const FVector NewLocation = FMath::VInterpConstantTo(Current, RespawnTargetLocation, DeltaTime, Speed);
			FHitResult SweepHit;
			RotateTurret(NewLocation, true);
			const bool bMoved = SetActorLocation(NewLocation, true, &SweepHit);
			return;
		}
	}
}

void ATower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!Tank) return;

	bool InSeenRange = isPlayerInRange();
	if (!InSeenRange) {
		GoToDeathLocation(DeltaTime);
		return;
	}

	const FVector TowerLocation = GetActorLocation();
	const FVector TargetLocation = Tank->GetActorLocation();

	const float VisibilityTolerance = 100.0f; 
	bool bHasLineOfSight = false;

	IgnoredActors.Add(this);


	//Added For Loop to ignore BlockVolumes by adding to IgnoredActors
	const int32 MaxIterations = 4;
	for (int32 Iter = 0; Iter < MaxIterations; ++Iter)
	{
		FHitResult Hit;
		FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(LineOfSight), true);
		QueryParams.AddIgnoredActors(IgnoredActors);
		QueryParams.bTraceComplex = true;

		const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, TowerLocation, TargetLocation, ECC_Visibility, QueryParams);

		// nothing hit -> clear line of sight
		if (!bHit)
		{
			bHasLineOfSight = true;
			break;
		}

		// If the first hit is the tank (or a component owned by it), we have LOS
		if (Hit.GetActor() == Tank || (Hit.GetComponent() && Hit.GetComponent()->GetOwner() == Tank))
		{
			bHasLineOfSight = true;
			break;
		}

		// If the hit is a blocking volume (editor-added ABlockingVolume) or another ignorable type,
		// ignore it and trace again. This allows traces to pass through editor-only volumes that
		// exist for navigation/level editing but shouldn't block "sight" for gameplay if desired.
		if (Hit.GetActor() && Hit.GetActor()->IsA<ABlockingVolume>())
		{
			IgnoredActors.Add(Hit.GetActor());
			continue; // trace again ignoring this blocking volume
		}

		// If the impact point is very close to the trace target, treat it as visible (trace ended inside the tank)
		const float ImpactDistToTarget = FVector::Dist(Hit.ImpactPoint, TargetLocation);
		if (ImpactDistToTarget <= VisibilityTolerance)
		{
			bHasLineOfSight = true;
			break;
		}

		// Hit something else that should block visibility -> no LOS
		bHasLineOfSight = false;
		break;
	}

	if (!bHasLineOfSight)
	{
		GoToDeathLocation(DeltaTime);
		return;
	}
	bHasRespawnTarget = false;
	bAtLastDeathLocation = false;
	RotateTurret(TargetLocation, true);

	const float Distance = FVector::Dist(TowerLocation, TargetLocation);

	if (Distance > FireRange)
	{
		const FVector NewLocation = FMath::VInterpConstantTo(TowerLocation, TargetLocation, DeltaTime, Speed);

		FHitResult SweepHit;
		const bool bMoved = SetActorLocation(NewLocation, true, &SweepHit);
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

void ATower::GetTowerSpawnLocationAndRotation(FVector& OutLocation, FRotator& OutRotation) const
{
	// Return the initial transform the tower had in the level (BeginPlay cached it).
	OutLocation = InitialSpawnTransform.GetLocation();
	OutRotation = InitialSpawnTransform.GetRotation().Rotator();
}