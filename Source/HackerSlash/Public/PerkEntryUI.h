// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PerkEntryUI.generated.h"

class UPerkEntryUI;
class UShopMenuUI;
class UButton;
class UTextBlock;
class UPerkDataAsset;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnBuyClickedNative, UPerkDataAsset*, UPerkEntryUI*);

UCLASS()
class HACKERSLASH_API UPerkEntryUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void InitPerk(UPerkDataAsset* InPerk);

	UFUNCTION(BlueprintCallable, Category="Shop")
	void BuyClicked();
	
	UFUNCTION(BlueprintCallable, Category="Shop")
	void SetOwnedMode(bool bOwned);
	
	FOnBuyClickedNative OnBuyClicked;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> PerkName;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> PerkDescription;
	
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> Purchase;

private:
	UPROPERTY()
	TObjectPtr<UPerkDataAsset> Perk;
};