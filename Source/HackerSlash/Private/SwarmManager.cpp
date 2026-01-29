// Fill out your copyright notice in the Description page of Project Settings.


#include "SwarmManager.h"

#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"

ASwarmManager::ASwarmManager()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.05f;
}

void ASwarmManager::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(TargetPawn))
	{
		TargetPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	}
}

void ASwarmManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CompactInvalid();
	PhaseDeg = FMath::Fmod(PhaseDeg + SwirlDegPerSec * DeltaTime, 360.f);
}

void ASwarmManager::RegisterEnemy(AEnemy* Enemy)
{
	if (!IsValid(Enemy)) return;

	CompactInvalid();

	if (!EnemyToSlot.Contains(Enemy))
	{
		const int32 Slot = EnemyToSlot.Num();
		EnemyToSlot.Add(Enemy, Slot);
		Enemies.Add(Enemy);
	}
}

void ASwarmManager::UnregisterEnemy(AEnemy* Enemy)
{
	if (!IsValid(Enemy)) return;

	Enemies.Remove(Enemy);
	EnemyToSlot.Remove(Enemy);
}

void ASwarmManager::CompactInvalid()
{
	for (int32 i = Enemies.Num() - 1; i >= 0; --i)
	{
		if (!IsValid(Enemies[i]))
		{
			EnemyToSlot.Remove(Enemies[i]);
			Enemies.RemoveAt(i);
		}
	}
}

int32 ASwarmManager::GetOrAssignSlot(const AEnemy* Enemy) const
{
	if (!IsValid(Enemy)) return INDEX_NONE;

	if (const int32* Found = EnemyToSlot.Find(const_cast<AEnemy*>(Enemy)))
	{
		return *Found;
	}
	
	return 0;
}

void ASwarmManager::ComputeRingForSlot(int32 Slot, float& OutRadius, float& OutAngleDeg) const
{
	// Compute how many enemies fit per ring 
	float Radius = BaseRadius;
	int32 Remaining = Slot;

	while (true)
	{
		const float Circ = 2.f * PI * Radius;
		const int32 Capacity = FMath::Max(6, (int32)FMath::FloorToInt(Circ / FMath::Max(10.f, DesiredArcSpacing)));

		if (Remaining < Capacity)
		{
			OutRadius = Radius;
			OutAngleDeg = (360.f / Capacity) * Remaining;
			return;
		}

		Remaining -= Capacity;
		Radius += RingSpacing;

		if (Radius > 10000.f)
		{
			OutRadius = Radius;
			OutAngleDeg = 0.f;
			return;
		}
	}
}

FVector ASwarmManager::GetGoalForEnemy(const AEnemy* Enemy) const
{
	if (!IsValid(TargetPawn))
	{
		return FVector::ZeroVector;
	}

	const FVector PlayerLoc = TargetPawn->GetActorLocation();
	const int32 Slot = GetOrAssignSlot(Enemy);

	float Radius = BaseRadius;
	float BaseAngle = 0.f;
	ComputeRingForSlot(Slot, Radius, BaseAngle);

	// Swirl direction optional: half clockwise, half counter-clockwise
	float DirSign = 1.f;
	if (bAlternateSwirlDirection && IsValid(Enemy))
	{
		DirSign = ((Enemy->GetUniqueID() & 1) == 0) ? 1.f : -1.f;
	}

	// Stable jitter per enemy
	float Jitter = 0.f;
	if (IsValid(Enemy) && AngleJitterDeg > 0.f)
	{
		const int32 J = (int32)(Enemy->GetUniqueID() % 11) - 5; // -5..+5
		Jitter = (float)J * (AngleJitterDeg / 5.f);
	}

	const float AngleDeg = BaseAngle + PhaseDeg * DirSign + Jitter;

	const FVector Dir = FRotationMatrix(FRotator(0.f, AngleDeg, 0.f)).GetUnitAxis(EAxis::X);
	const FVector Desired = PlayerLoc + Dir * Radius;

	UWorld* World = GetWorld();
	if (!World) return Desired;

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
	if (!NavSys) return Desired;

	FNavLocation Out;
	return NavSys->ProjectPointToNavigation(Desired, Out, NavProjectExtent) ? Out.Location : Desired;
}