// Fill out your copyright notice in the Description page of Project Settings.


#include "HackNSlashMode.h"

#include "BP_Player.h"
#include "Enemy.h"
#include "Kismet/GameplayStatics.h"

AHackNSlashMode::AHackNSlashMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClass(TEXT("/Game/Blueprints/Player/MainChar.MainChar_C"));
	DefaultPawnClass = PlayerPawnClass.Class;
}

void AHackNSlashMode::BeginPlay()
{
	Super::BeginPlay();

	PlayerPawnRef = Cast<ABP_Player>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	StartNextWave();
}

void AHackNSlashMode::StartNextWave()
{
	CurrentWaveIndex++;

	if (!Waves.IsValidIndex(CurrentWaveIndex))
	{
		CurrentWaveIndex = 0;
	}

	CurrentWave = Waves[CurrentWaveIndex];

	if (CurrentWave.WaveType == EWaveType::Shop)
	{
		StartShopWave(CurrentWave);
		return;
	}

	StartCombatWave(CurrentWave);
}

void AHackNSlashMode::StartCombatWave(const FWaveDefinition& WaveDef)
{
	EnemiesToSpawnThisWave = WaveDef.EnemyCount;
	EnemiesSpawnedThisWave = 0;

	GetWorldTimerManager().ClearTimer(WaveSpawnTimer);
	GetWorldTimerManager().SetTimer(
		WaveSpawnTimer,
		this,
		&AHackNSlashMode::SpawnEnemyFromPlayerPoint,
		WaveDef.TimeBetweenSpawns,
		true
	);
}

void AHackNSlashMode::StartShopWave(const FWaveDefinition& WaveDef)
{
	GetWorldTimerManager().ClearTimer(WaveSpawnTimer);

	OnShopWaveStarted.Broadcast(CurrentWaveIndex + 1);

	GetWorldTimerManager().SetTimer(
		WaveEndTimer,
		this,
		&AHackNSlashMode::StartNextWave,
		WaveDef.ShopDuration,
		false
	);
}

void AHackNSlashMode::SpawnEnemyFromPlayerPoint()
{
	if (EnemiesSpawnedThisWave >= EnemiesToSpawnThisWave)
	{
		GetWorldTimerManager().ClearTimer(WaveSpawnTimer);
		return;
	}

	if (!PlayerPawnRef) return;

	TSubclassOf<AEnemy> EnemyToSpawn = PickEnemyForWave(CurrentWave);
	if (!EnemyToSpawn) return;

	AActor* SpawnedEnemy = PlayerPawnRef->SpawnEnemy(EnemyToSpawn);
	if (!SpawnedEnemy) return;

	EnemiesSpawnedThisWave++;
	AliveEnemies++;

	SpawnedEnemy->OnDestroyed.AddDynamic(this, &AHackNSlashMode::HandleEnemyDestroyed);
}

void AHackNSlashMode::HandleEnemyDestroyed(AActor* DestroyedActor) // OnDestroyed expects an actor
{
	AliveEnemies = FMath::Max(0, AliveEnemies - 1);

	const bool bAllSpawned = (EnemiesSpawnedThisWave >= EnemiesToSpawnThisWave);
	if (bAllSpawned && AliveEnemies == 0)
	{
		GetWorldTimerManager().SetTimer(
			WaveEndTimer,
			this,
			&AHackNSlashMode::StartNextWave,
			TimeBetweenWaves,
			false
		);
	}
}

TSubclassOf<AEnemy> AHackNSlashMode::PickEnemyForWave(const FWaveDefinition& WaveDef) const // More weight, more frequency
{
	TArray<const FEnemySpawnEntry*> Candidates;
	float TotalWeight = 0.f;

	for (const FEnemySpawnEntry& E : WaveDef.SpawnTable)
	{
		if (!E.EnemyClass) continue;
		if (CurrentWaveIndex + 1 < E.MinWave) continue;
		if (CurrentWaveIndex + 1 > E.MaxWave) continue;

		Candidates.Add(&E);
		TotalWeight += FMath::Max(0.f, E.Weight);
	}

	if (Candidates.Num() == 0 || TotalWeight <= 0.f)
	{
		return nullptr;
	}

	const float Roll = FMath::FRandRange(0.f, TotalWeight);
	float Running = 0.f;

	for (const FEnemySpawnEntry* Entry : Candidates)
	{
		Running += FMath::Max(0.f, Entry->Weight);
		if (Roll <= Running)
		{
			return Entry->EnemyClass;
		}
	}

	return Candidates.Last()->EnemyClass;
}