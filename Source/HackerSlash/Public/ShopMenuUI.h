// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShopMenuUI.generated.h"

class UPerkEntryUI;
class UScrollBox;
class UPerkDataAsset;
class UPerkComponent;

DECLARE_MULTICAST_DELEGATE(FOnShopClosedNative);

UCLASS()
class HACKERSLASH_API UShopMenuUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Shop")
	void InitShop(const TArray<UPerkDataAsset*>& InOffer, UPerkComponent* InPerkComp);

	// Owned-only view
	UFUNCTION(BlueprintCallable, Category="Shop")
	void InitOwnedOnly(UPerkComponent* InPerkComp);

	UFUNCTION(BlueprintCallable, Category="Shop")
	void CloseClicked();

	FOnShopClosedNative OnShopClosed;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UScrollBox> PerkList;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UScrollBox> OwnedPerkList;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Shop")
	TSubclassOf<UPerkEntryUI> PerkEntryWidgetClass;

private:
	UPROPERTY()
	TObjectPtr<UPerkComponent> PerkComp;
	
	void PopulateOffer(const TArray<UPerkDataAsset*>& InOffer);
	void PopulateOwned();
	void AddOwnedEntry(UPerkDataAsset* Perk);
	
	void HandleBuyPerk(UPerkDataAsset* Perk, UPerkEntryUI* EntryWidget);
};