// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopMenuUI.h"

#include "HackNSlashMode.h"
#include "PerkComponent.h"
#include "PerkDataAsset.h"
#include "PerkEntryUI.h"
#include "Components/ScrollBox.h"
#include "Kismet/GameplayStatics.h"


class AHackNSlashMode;

void UShopMenuUI::InitShop(const TArray<UPerkDataAsset*>& InOffer, UPerkComponent* InPerkComp)
{
	PerkComp = InPerkComp;

	if (PerkList)
	{
		PerkList->ClearChildren();
	}
	
	if (OwnedPerkList)
	{
		OwnedPerkList->ClearChildren();
	}
	
	PopulateOffer(InOffer);
	PopulateOwned();
}

void UShopMenuUI::InitOwnedOnly(UPerkComponent* InPerkComp)
{
	PerkComp = InPerkComp;

	if (PerkList)
	{
		PerkList->ClearChildren();
		PerkList->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (OwnedPerkList)
	{
		OwnedPerkList->ClearChildren();
		OwnedPerkList->SetVisibility(ESlateVisibility::Visible);
	}

	PopulateOwned();
}


void UShopMenuUI::PopulateOffer(const TArray<UPerkDataAsset*>& InOffer)
{
	if (!PerkList || !PerkEntryWidgetClass) return;

	APlayerController* PlayerController = GetOwningPlayer();
	if (!PlayerController) return;

	for (UPerkDataAsset* Perk : InOffer)
	{
		if (!IsValid(Perk)) continue;

		UPerkEntryUI* Entry = CreateWidget<UPerkEntryUI>(PlayerController, PerkEntryWidgetClass);
		if (!Entry) continue;

		Entry->InitPerk(Perk);
		Entry->SetOwnedMode(false);
		Entry->OnBuyClicked.AddUObject(this, &UShopMenuUI::HandleBuyPerk);

		PerkList->AddChild(Entry);
	}
}

void UShopMenuUI::PopulateOwned()
{
	if (!OwnedPerkList || !PerkEntryWidgetClass) return;
	if (!PerkComp) return;

	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	for (UPerkDataAsset* Owned : PerkComp->OwnedPerks)
	{
		if (!IsValid(Owned)) continue;

		UPerkEntryUI* Entry = CreateWidget<UPerkEntryUI>(PC, PerkEntryWidgetClass);
		if (!Entry) continue;

		Entry->InitPerk(Owned);
		Entry->SetOwnedMode(true);

		OwnedPerkList->AddChild(Entry);
	}
}

void UShopMenuUI::AddOwnedEntry(UPerkDataAsset* Perk)
{
	if (!OwnedPerkList || !PerkEntryWidgetClass) return;
	if (!IsValid(Perk)) return;

	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	UPerkEntryUI* Entry = CreateWidget<UPerkEntryUI>(PC, PerkEntryWidgetClass);
	if (!Entry) return;

	Entry->InitPerk(Perk);
	Entry->SetOwnedMode(true);

	OwnedPerkList->AddChild(Entry);
}
void UShopMenuUI::HandleBuyPerk(UPerkDataAsset* Perk, UPerkEntryUI* EntryWidget)
{
	if (!IsValid(Perk) || !IsValid(EntryWidget)) return;

	APlayerController* PlayerController = GetOwningPlayer();
	APawn* Pawn = PlayerController ? PlayerController->GetPawn() : nullptr;
	if (!Pawn) return;

	UPerkComponent* Perks = Pawn->FindComponentByClass<UPerkComponent>();
	if (!Perks) return;

	if (Perks->ApplyPerk(Perk))
	{
		if (AHackNSlashMode* GameManager = Cast<AHackNSlashMode>(UGameplayStatics::GetGameMode(this)))
		{
			GameManager->RemovePurchasablePerk(Perk);
		}

		EntryWidget->RemoveFromParent();
		AddOwnedEntry(Perk);

		OnShopClosed.Broadcast();
	}
}

void UShopMenuUI::CloseClicked()
{
	OnShopClosed.Broadcast();
}
