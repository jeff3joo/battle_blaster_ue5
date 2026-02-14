// Fill out your copyright notice in the Description page of Project Settings.

#include "Tower.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/BlockingVolume.h"

void ATower::BeginPlay()
{
	Super::BeginPlay();

	// Ensure the root component has collision enabled so sweeping actually blocks movement.
	if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(GetRootComponent()))
	{
		if (Prim->GetCollisionEnabled() == ECollisionEnabled::NoCollision)
		{
			Prim->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			Prim->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
			UE_LOG(LogTemp, Warning, TEXT("Tower: Root collision was disabled, enabling QueryAndPhysics and blocking all channels."));
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

void ATower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!Tank) return;

	bool InSeenRange = isPlayerInRange();
	if (!InSeenRange) return;

	const FVector TowerLocation = GetActorLocation();
	// Aim at a point near the tank's center/eyes to avoid the trace ending inside the tank's root.
	const FVector TargetLocation = Tank->GetActorLocation();// +FVector(0.f, 0.f, 50.f);

	// Robust line-of-sight: perform iterative traces and ignore non-physical blocking volumes (or other ignorable actors)
	// until either we confirm the Tank is first, or a blocking actor that should stop visibility remains.
	const float VisibilityTolerance = 100.0f; // tweak if necessary
	bool bHasLineOfSight = false;

	IgnoredActors.Add(this);


	//Added For Loop to ignore BlockVolumes by adding to IgnoredActors
	const int32 MaxIterations = 8;
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

	// Debug visualization: green = visible, red = blocked
	DrawDebugLine(GetWorld(), TowerLocation, TargetLocation, bHasLineOfSight ? FColor::Green : FColor::Red, false, 0.1f, 0, 2.0f);

	if (!bHasLineOfSight)
	{
		return;
	}

	// We can see the tank: rotate turret and attempt to move, but use a sweep and inspect sweep hit.
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
