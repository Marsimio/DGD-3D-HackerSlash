// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HackNSlashMode.generated.h"

class UShopMenuUI;
class UPerkDataAsset;
class AEnemy;
class ABP_Player;

UENUM(BlueprintType)
enum class EWaveType : uint8
{
	Combat,
	Shop
};

USTRUCT(BlueprintType)
struct FEnemySpawnEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AEnemy> EnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0.0"))
	float Weight = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="1"))
	int32 MinWave = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="1"))
	int32 MaxWave = 9999;
};

USTRUCT(BlueprintType)
struct FWaveDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWaveType WaveType = EWaveType::Combat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", EditCondition="WaveType==EWaveType::Combat"))
	int32 EnemyCount = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0.01", EditCondition="WaveType==EWaveType::Combat"))
	float TimeBetweenSpawns = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="WaveType==EWaveType::Combat"))
	TArray<FEnemySpawnEntry> SpawnTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0.0", EditCondition="WaveType==EWaveType::Shop"))
	float ShopDuration = 20.f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWaveStarted, int32, WaveIndex1Based, EWaveType, WaveType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveEnded, int32, WaveIndex1Based);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShopWaveStarted, int32, WaveIndex1Based);

UCLASS()
class HACKERSLASH_API AHackNSlashMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AHackNSlashMode();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Shop")
	TSubclassOf<UUserWidget> ShopMenuClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Shop")
	TArray<TObjectPtr<UPerkDataAsset>> PurchasablePerks;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Shop", meta=(ClampMin="1"))
	int32 PerksOfferedPerShop = 3;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
	TArray<FWaveDefinition> Waves;
	
	UPROPERTY(BlueprintAssignable, Category="Waves")
	FOnWaveStarted OnWaveStarted;

	UPROPERTY(BlueprintAssignable, Category="Waves")
	FOnWaveEnded OnWaveEnded;

	UPROPERTY(BlueprintAssignable, Category="Waves")
	FOnShopWaveStarted OnShopWaveStarted;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Waves")
	float TimeBetweenWaves = 2.0f;
	
	UFUNCTION(BlueprintCallable, Category="Waves")
	void StartNextWave();

	UFUNCTION(BlueprintCallable)
	void EndShopWave()
	{
		GetWorldTimerManager().ClearTimer(WaveEndTimer);
		StartNextWave();
	}
	
	UFUNCTION(BlueprintCallable, Category="Shop")
	void RemovePurchasablePerk(UPerkDataAsset* Perk);
	
	void EndShopWaveEarly();
		
private:
	
	virtual void BeginPlay() override;
	
	UPROPERTY()
	TObjectPtr<UShopMenuUI> ActiveShopWidget;
	UPROPERTY()
	ABP_Player* PlayerPawnRef;
	
	UFUNCTION()
	void SpawnEnemyFromPlayerPoint();
	UFUNCTION()
	void HandleEnemyDestroyed(AActor* DestroyedActor);
	UFUNCTION()
	void OpenOwnedPerksUI();
	
	int32 CurrentWaveIndex = -1;
	FWaveDefinition CurrentWave;

	int32 EnemiesToSpawnThisWave = 0;
	int32 EnemiesSpawnedThisWave = 0;
	int32 AliveEnemies = 0;

	FTimerHandle WaveSpawnTimer;
	FTimerHandle WaveEndTimer;

	void StartCombatWave(const FWaveDefinition& WaveDef);
	void StartShopWave(const FWaveDefinition& WaveDef);
	
	TArray<UPerkDataAsset*> BuildShopOffer() const;
	void OpenShopUI();
	void CloseShopUI();

	TSubclassOf<AEnemy> PickEnemyForWave(const FWaveDefinition& WaveDef) const;

};
