// Fill out your copyright notice in the Description page of Project Settings.


#include "HackNSlashMode.h"

#include "BP_Player.h"
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

    int32 NumEnemies = EnemiesPerWave * CurrentWave;

    for (int32 i = 0; i < NumEnemies; i++)
    {
        SpawnEnemyFromPlayerPoint();
    }
}

void AHackNSlashMode::SpawnEnemyFromPlayerPoint()
{
	if (PlayerPawnRef && EnemyBlueprintClass)
	{
		PlayerPawnRef->SpawnEnemy(EnemyBlueprintClass);
	}
}