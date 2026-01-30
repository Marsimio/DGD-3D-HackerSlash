// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopMenuUI.h"

#include "HackNSlashMode.h"
#include "PerkComponent.h"
#include "PerkDataAsset.h"
#include "PerkEntryUI.h"
#include "Components/ScrollBox.h"
#include "Kismet/GameplayStatics.h"


class AHackNSlashMode;

void UShopMenuUI::Init(const TArray<UPerkDataAsset*>& InPerks)
{
	if (!PerkList || !PerkEntryWidgetClass) return;

	PerkList->ClearChildren();

	APlayerController* PlayerController = GetOwningPlayer();
	if (!PlayerController) return;

	for (UPerkDataAsset* Perk : InPerks)
	{
		if (!IsValid(Perk)) continue;

		UPerkEntryUI* Entry = CreateWidget<UPerkEntryUI>(PlayerController, PerkEntryWidgetClass);
		if (!Entry) continue;

		Entry->InitPerk(Perk);
		Entry->OnBuyClicked.AddUObject(this, &UShopMenuUI::HandleBuyPerk);

		PerkList->AddChild(Entry);
	}
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
		if (AHackNSlashMode* GM = Cast<AHackNSlashMode>(UGameplayStatics::GetGameMode(this)))
		{
			GM->RemovePurchasablePerk(Perk);
		}

		EntryWidget->RemoveFromParent();
		OnShopClosed.Broadcast(); // left here until proper implementation
	}
}

void UShopMenuUI::CloseClicked()
{
	OnShopClosed.Broadcast();
}