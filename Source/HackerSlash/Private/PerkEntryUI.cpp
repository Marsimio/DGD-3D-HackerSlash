// Fill out your copyright notice in the Description page of Project Settings.


#include "PerkEntryUI.h"
#include "PerkDataAsset.h"
#include "Components/TextBlock.h"

void UPerkEntryUI::InitPerk(UPerkDataAsset* InPerk)
{
	Perk = InPerk;
	if (!Perk) return;

	if (PerkName)
	{
		PerkName->SetText(Perk->Name);
	}

	if (PerkDescription)
	{
		PerkDescription->SetText(Perk->Description);
	}
}

void UPerkEntryUI::BuyClicked()
{
	if (!Perk) return;
	OnBuyClicked.Broadcast(Perk, this);
}