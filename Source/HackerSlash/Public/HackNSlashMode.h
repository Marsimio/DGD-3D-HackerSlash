// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HackNSlashMode.generated.h"

/**
 * 
 */
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
	TSubclassOf<AActor> EnemyBlueprintClass;
	
	UPROPERTY()
	class ABP_Player* PlayerPawnRef;
		
	FTimerHandle EnemySpawnTimer;
	
	
protected:
	virtual void BeginPlay() override;

	
public:
	
	void StartNextWave();
	void SpawnEnemyFromPlayerPoint();
	
};
