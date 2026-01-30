// Fill out your copyright notice in the Description page of Project Settings.


#include "HackNSlashMode.h"

#include "BP_Player.h"
#include "Enemy.h"
#include "HealthComponent.h"
#include "PerkComponent.h"
#include "PerkDataAsset.h"
#include "ShopMenuUI.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
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

	if (PlayerPawnRef)
	{
		PlayerPawnRef->OnOwnedPerksRequested.AddUObject(this, &AHackNSlashMode::OpenOwnedPerksUI);
	}
	StartNextWave();
}

void AHackNSlashMode::StartNextWave()
{
	if (CurrentWaveIndex >= 0)
	{
		OnWaveEnded.Broadcast(CurrentWaveIndex + 1);
	}
	
	CurrentWaveIndex++;

	if (!Waves.IsValidIndex(CurrentWaveIndex))
	{
		CurrentWaveIndex = 0;
	}

	CurrentWave = Waves[CurrentWaveIndex];

	if (CurrentWave.WaveType == EWaveType::Shop)
	{
		StartShopWave(CurrentWave);
	}

	StartCombatWave(CurrentWave);
}

void AHackNSlashMode::StartCombatWave(const FWaveDefinition& WaveDef)
{
	PlayerPawnRef->HealthComp->Heal(9999);
	EnemiesToSpawnThisWave = WaveDef.EnemyCount;
	EnemiesSpawnedThisWave = 0;
	AliveEnemies = 0;

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
	GetWorldTimerManager().ClearTimer(WaveEndTimer);

	OnShopWaveStarted.Broadcast(CurrentWaveIndex + 1);

	OpenShopUI();

	GetWorldTimerManager().SetTimer(
		WaveEndTimer,
		this,
		&AHackNSlashMode::EndShopWaveEarly,
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
		GetWorldTimerManager().ClearTimer(WaveEndTimer);
		GetWorldTimerManager().SetTimer(
			WaveEndTimer,
			this,
			&AHackNSlashMode::StartNextWave,
			TimeBetweenWaves,
			false
		);
	}
}

void AHackNSlashMode::OpenOwnedPerksUI()
{
	if (!ShopMenuClass) return;

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (!PlayerController || !PlayerPawnRef) return;

	UPerkComponent* PerkComp = PlayerPawnRef->FindComponentByClass<UPerkComponent>();
	if (!PerkComp) return;

	if (ActiveShopWidget)
	{
		CloseShopUI();
		return;
	}

	UShopMenuUI* ShopWidget = CreateWidget<UShopMenuUI>(PlayerController, ShopMenuClass);
	if (!ShopWidget) return;

	ActiveShopWidget = ShopWidget;
	ShopWidget->OnShopClosed.AddUObject(this, &AHackNSlashMode::CloseShopUI);
	ShopWidget->AddToViewport();

	ShopWidget->InitOwnedOnly(PerkComp);

	FInputModeUIOnly Mode;
	Mode.SetWidgetToFocus(ShopWidget->TakeWidget());
	Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PlayerController->SetInputMode(Mode);
	PlayerController->bShowMouseCursor = true;
}

TArray<UPerkDataAsset*> AHackNSlashMode::BuildShopOffer() const
{
	TArray<UPerkDataAsset*> PerkPool;
	for (UPerkDataAsset* Perk : PurchasablePerks)
	{
		if (IsValid(Perk))
		{
			PerkPool.Add(Perk);
		}
	}

	TArray<UPerkDataAsset*> Offer;
	const int32 NumToPick = FMath::Min(PerksOfferedPerShop, PerkPool.Num());
	for (int32 i = 0; i < NumToPick; ++i)
	{
		const int32 Index = FMath::RandRange(0, PerkPool.Num() - 1);
		Offer.Add(PerkPool[Index]);
		PerkPool.RemoveAtSwap(Index);
	}

	return Offer;
}

void AHackNSlashMode::OpenShopUI()
{
	if (!ShopMenuClass) return;

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (!PlayerController) return;

	UShopMenuUI* ShopWidget = CreateWidget<UShopMenuUI>(PlayerController, ShopMenuClass);
	if (!ShopWidget) return;
	
	ActiveShopWidget = ShopWidget;
	ShopWidget->OnShopClosed.AddUObject(this, &AHackNSlashMode::EndShopWaveEarly);
	ShopWidget->AddToViewport();
	APawn* Pawn = PlayerController->GetPawn();
	UPerkComponent* PerkComp = Pawn ? Pawn->FindComponentByClass<UPerkComponent>() : nullptr;

	ShopWidget->InitShop(BuildShopOffer(), PerkComp);

	FInputModeUIOnly Mode;
	Mode.SetWidgetToFocus(ShopWidget->TakeWidget());
	Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PlayerController->SetInputMode(Mode);
	PlayerController->bShowMouseCursor = true;
}

void AHackNSlashMode::CloseShopUI()
{
	if (ActiveShopWidget)
	{
		ActiveShopWidget->RemoveFromParent();
		ActiveShopWidget = nullptr;
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PlayerController) return;

	PlayerController->SetInputMode(FInputModeGameOnly());
	PlayerController->bShowMouseCursor = false;

	PlayerController->bEnableClickEvents = false;
	PlayerController->bEnableMouseOverEvents = false;

	PlayerController->SetIgnoreMoveInput(false);
	PlayerController->SetIgnoreLookInput(false);

	UWidgetBlueprintLibrary::SetFocusToGameViewport();
}

TSubclassOf<AEnemy> AHackNSlashMode::PickEnemyForWave(const FWaveDefinition& WaveDef) const // More weight, more frequency
{
	TArray<const FEnemySpawnEntry*> Candidates;
	float TotalWeight = 0.f;

	for (const FEnemySpawnEntry& EnemyEntry : WaveDef.SpawnTable)
	{
		if (!EnemyEntry.EnemyClass) continue;
		if (CurrentWaveIndex + 1 < EnemyEntry.MinWave) continue;
		if (CurrentWaveIndex + 1 > EnemyEntry.MaxWave) continue;

		Candidates.Add(&EnemyEntry);
		TotalWeight += FMath::Max(0.f, EnemyEntry.Weight);
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

void AHackNSlashMode::RemovePurchasablePerk(UPerkDataAsset* Perk)
{
	if (!IsValid(Perk)) return;

	PurchasablePerks.RemoveAll([Perk](UPerkDataAsset* PerkToRemove)
	{
		return PerkToRemove == Perk;
	});
}

void AHackNSlashMode::EndShopWaveEarly()
{
	GetWorldTimerManager().ClearTimer(WaveEndTimer);

	CloseShopUI();

	StartNextWave();
}
