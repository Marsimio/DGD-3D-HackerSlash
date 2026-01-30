// Fill out your copyright notice in the Description page of Project Settings.


#include "PerkComponent.h"

#include "PerkDataAsset.h"
#include "StatsComponent.h"

UPerkComponent::UPerkComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPerkComponent::BeginPlay()
{
	Super::BeginPlay();
	StatsComp = GetOwner() ? GetOwner()->FindComponentByClass<UStatsComponent>() : nullptr;
}

bool UPerkComponent::HasPerkId(FName PerkId) const
{
	for (const UPerkDataAsset* OwnedPerk : OwnedPerks)
	{
		if (!IsValid(OwnedPerk)) continue;
		if (OwnedPerk->PerkId == PerkId) return true;
	}
	return false;
}

bool UPerkComponent::ApplyPerk(UPerkDataAsset* Perk)
{
	if (!IsValid(Perk)) return false;

	const FName Id = (Perk->PerkId != NAME_None) ? Perk->PerkId : FName(*Perk->GetName());
	if (HasPerkId(Id)) return false;

	OwnedPerks.Add(Perk);

	if (StatsComp)
	{
		StatsComp->AddPerk(Perk);
	}

	return true;
}