// Fill out your copyright notice in the Description page of Project Settings.


#include "StatsComponent.h"

#include "PerkDataAsset.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/FloatingPawnMovement.h"


class UFloatingPawnMovement;
class UCharacterMovementComponent;

UStatsComponent::UStatsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UStatsComponent::BeginPlay()
{
	Super::BeginPlay();

	RebuildStats();
}

float& UStatsComponent::StatRef(FStatBlock& Block, EGameStatType Stat) // References current stats
{
	switch (Stat)
	{
	case EGameStatType::MoveSpeed:      return Block.MoveSpeed;
	case EGameStatType::Damage:         return Block.Damage;
	case EGameStatType::AttackCooldown: return Block.AttackCooldown;
	case EGameStatType::MaxHealth:      return Block.MaxHealth;
	default:                        return Block.MoveSpeed;
	}
}

float UStatsComponent::StatVal(const FStatBlock& Block, EGameStatType Stat) // References base stats for multipliers and bonuses
{
	switch (Stat)
	{
	case EGameStatType::MoveSpeed:      return Block.MoveSpeed;
	case EGameStatType::Damage:         return Block.Damage;
	case EGameStatType::AttackCooldown: return Block.AttackCooldown;
	case EGameStatType::MaxHealth:      return Block.MaxHealth;
	default:                        return Block.MoveSpeed;
	}
}

float UStatsComponent::GetStat(EGameStatType Stat) const
{
	return StatVal(FinalStats, Stat);
}

bool UStatsComponent::HasPerk(FName PerkId) const // Used to validate if they have X perk already
{
	for (const FAppliedPerk& AppliedPerk : AppliedPerks) // For every perk being applied
	{
		if (AppliedPerk.PerkId == PerkId)
		{
			return true;
		}
	}
	return false;
}

bool UStatsComponent::AddPerk(UPerkDataAsset* Perk) // Usually called by stat component after one of its perks is added
{
	if (!IsValid(Perk)) return false;

	const FName Id = (Perk->PerkId != NAME_None) ? Perk->PerkId : FName(*Perk->GetName());

	if (HasPerk(Id)) return false;

	FAppliedPerk NewPerk;
	NewPerk.Perk = Perk;
	NewPerk.PerkId = Id;
	AppliedPerks.Add(NewPerk);

	RebuildStats();
	return true;
}
bool UStatsComponent::RemovePerkById(FName PerkId) // gone used and unimplemented in PerkComponent
{
	for (int32 i = 0; i < AppliedPerks.Num(); ++i)
	{
		if (AppliedPerks[i].PerkId == PerkId)
		{
			AppliedPerks.RemoveAt(i);
			RebuildStats();
			return true;
		}
	}
	return false;
}

void UStatsComponent::ClearPerks() // Gone unused
{
	AppliedPerks.Reset();
	RebuildStats();
}

void UStatsComponent::RebuildStats() // To be called when needed (BeginPlay/ Stat Changes). Could be used for debuffs down the line
{
	FinalStats = BaseStats;

	struct FAcc
	{
		float Add = 0.f;
		float Mul = 1.f;
	};

	TMap<EGameStatType, FAcc> Accumulation;
	Accumulation.Add(EGameStatType::MoveSpeed, {});
	Accumulation.Add(EGameStatType::Damage, {});
	Accumulation.Add(EGameStatType::AttackCooldown,{});
	Accumulation.Add(EGameStatType::MaxHealth,{});

	for (const FAppliedPerk& AppliedPerk : AppliedPerks)
	{
		if (!IsValid(AppliedPerk.Perk)) continue;

		for (const FStatMod& Modifier : AppliedPerk.Perk->Mods)
		{
			FAcc& AccumulationValue = Accumulation.FindOrAdd(Modifier.Stat);

			if (Modifier.Op == EModOp::Add)
			{
				AccumulationValue.Add += Modifier.Value;
			}
			else
			{
				AccumulationValue.Mul *= Modifier.Value;
			}
		}
	}

	for (auto& Pair : Accumulation)
	{
		const EGameStatType Stat = Pair.Key;
		const FAcc& FinalMod = Pair.Value;

		float& Out = StatRef(FinalStats, Stat);
		Out = (Out + FinalMod.Add) * FinalMod.Mul;
	}

	ApplyMoveSpeedToMovement();
	OnStatsChanged.Broadcast();
}

void UStatsComponent::ApplyMoveSpeedToMovement()
{
	if (!bAutoApplyMoveSpeed) return;

	AActor* Owner = GetOwner();
	if (!Owner) return;

	const float Speed = FinalStats.MoveSpeed;

	// CharacterMovement for enemies
	if (bApplyToCharacterMovement)
	{
		if (ACharacter* Character = Cast<ACharacter>(Owner))
		{
			if (UCharacterMovementComponent* Move = Character->GetCharacterMovement())
			{
				Move->MaxWalkSpeed = Speed;
			}
		}
	}

	// PawnMovement for players
	if (bApplyToFloatingPawnMovement)
	{
		if (UFloatingPawnMovement* FloatMove = Owner->FindComponentByClass<UFloatingPawnMovement>())
		{
			FloatMove->MaxSpeed = Speed;
		}
	}
}