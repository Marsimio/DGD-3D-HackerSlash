// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"

#include "StatsComponent.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Owner = GetOwner())
	{
		StatsComp = Owner->FindComponentByClass<UStatsComponent>();
		if (StatsComp && bUseStatsMaxHealth)
		{
			StatsComp->OnStatsChanged.AddDynamic(this, &UHealthComponent::HandleStatsChanged);
			HandleStatsChanged(); // sync max + current at start
		}
		else
		{
			Health = MaxHealth; // fallback behavior
		}

		Owner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleTakeAnyDamage);
	}
}

void UHealthComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (StatsComp && bUseStatsMaxHealth)
	{
		StatsComp->OnStatsChanged.RemoveDynamic(this, &UHealthComponent::HandleStatsChanged);
	}

	Super::EndPlay(EndPlayReason);
}

void UHealthComponent::HandleStatsChanged()
{
	if (!StatsComp) return;

	const float NewMax = FMath::Max(1.f, StatsComp->GetFinalStats().MaxHealth);

	const float OldMax = MaxHealth;
	const float OldHealth = Health;

	MaxHealth = NewMax;

	if (bKeepHealthPercentOnMaxHealthChange && OldMax > 0.f)
	{
		const float Pct = OldHealth / OldMax;
		Health = FMath::Clamp(Pct * MaxHealth, 0.f, MaxHealth);
	}
	else
	{
		Health = FMath::Min(Health, MaxHealth);
	}

	OnHealthChanged.Broadcast(this, Health, Health - OldHealth, nullptr);
}

void UHealthComponent::Heal(float fAmount)
{
	if (fAmount <= 0.f || IsDead()) return;

	const float Old = Health;
	Health = FMath::Clamp(Health + fAmount, 0.f, MaxHealth);

	OnHealthChanged.Broadcast(this, Health, Health - Old, nullptr);
}

void UHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage,
                                           const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.f || IsDead()) return;

	const float Old = Health;
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);

	OnHealthChanged.Broadcast(this, Health, Health - Old, DamageCauser);

	if (Health <= 0.f)
	{
		OnDied.Broadcast(DamagedActor);
	}
}
