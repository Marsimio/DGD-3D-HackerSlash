// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HackNSlashMode.generated.h"

class AEnemy;
class ABP_Player;

UCLASS()
class HACKERSLASH_API AHackNSlashMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AHackNSlashMode();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
	int32 CurrentWave = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
	int32 EnemiesPerWave = 5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Waves")
	TSubclassOf<AEnemy> EnemyBlueprintClass; // Subclass of enemy to allow for variants

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
	float TimeBetweenSpawns = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
	float TimeBetweenWaves = 2.0f;
	
	UPROPERTY()
	ABP_Player* PlayerPawnRef;
		
private:
	FTimerHandle EnemySpawnTimer;
	int32 EnemiesToSpawnThisWave = 0;
	int32 EnemiesSpawnedThisWave = 0;
	int32 AliveEnemies = 0;
	
	
protected:
	virtual void BeginPlay() override;

	
public:
	UFUNCTION()
	void StartNextWave();
	UFUNCTION()
	void SpawnEnemyFromPlayerPoint();
	UFUNCTION()
	void HandleEnemyDestroyed(AActor* DestroyedActor);
	
};
