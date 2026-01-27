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
    CurrentWave++;

	EnemiesToSpawnThisWave = EnemiesPerWave * CurrentWave;
	EnemiesSpawnedThisWave = 0;

	GetWorldTimerManager().ClearTimer(EnemySpawnTimer);
	
	GetWorldTimerManager().SetTimer(
	EnemySpawnTimer,
	this,
	&AHackNSlashMode::SpawnEnemyFromPlayerPoint,
	TimeBetweenSpawns,
	true
);
}

void AHackNSlashMode::SpawnEnemyFromPlayerPoint()
{
	if (EnemiesSpawnedThisWave >= EnemiesToSpawnThisWave)
	{
		GetWorldTimerManager().ClearTimer(EnemySpawnTimer); 
		return;
	}
	
	if (PlayerPawnRef && EnemyBlueprintClass)
	{
		AActor* SpawnedEnemy = PlayerPawnRef->SpawnEnemy(EnemyBlueprintClass);
		if (SpawnedEnemy)
		{
			EnemiesSpawnedThisWave++;
			AliveEnemies++;
		
			SpawnedEnemy->OnDestroyed.AddDynamic(this, &AHackNSlashMode::HandleEnemyDestroyed);
		}
	}
}

void AHackNSlashMode::HandleEnemyDestroyed(AActor* DestroyedActor)
{
	AliveEnemies = FMath::Max(0, AliveEnemies - 1);
	
	const bool bAllEnemiesSpawned = (EnemiesSpawnedThisWave >= EnemiesToSpawnThisWave);
	
	if (bAllEnemiesSpawned && AliveEnemies == 0)
	{
		{
			FTimerHandle NextWaveTimer; //uninitialized
			GetWorldTimerManager().SetTimer(
				NextWaveTimer,
				this,
				&AHackNSlashMode::StartNextWave,
				TimeBetweenWaves,
				false
			);
		}
	}
}
