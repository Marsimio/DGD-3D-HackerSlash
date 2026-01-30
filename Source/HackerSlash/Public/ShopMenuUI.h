// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShopMenuUI.generated.h"

class UPerkEntryUI;
class UScrollBox;
class UPerkDataAsset;

DECLARE_MULTICAST_DELEGATE(FOnShopClosedNative);

UCLASS()
class HACKERSLASH_API UShopMenuUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Shop")
	void Init(const TArray<UPerkDataAsset*>& InPerks);

	UFUNCTION(BlueprintCallable, Category="Shop")
	void CloseClicked();
	
	FOnShopClosedNative OnShopClosed;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UScrollBox> PerkList;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Shop")
	TSubclassOf<UPerkEntryUI> PerkEntryWidgetClass;

private:
	void HandleBuyPerk(UPerkDataAsset* Perk, UPerkEntryUI* EntryWidget);
};